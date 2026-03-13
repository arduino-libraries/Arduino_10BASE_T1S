/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "TC6_Arduino_10BASE_T1S.h"

#include "lib/libtc6/inc/tc6-regs.h"

#if defined(ARDUINO_ARCH_RP2040)
/* Use the lwIP stack supplied by the arduino-pico core (NO_SYS=1). */
#include <lwip/init.h>
#include <lwip/timeouts.h>
#include <lwip/dhcp.h>
#include <netif/etharp.h>
#else
#include "lib/liblwip/include/lwip/init.h"
#include "lib/liblwip/include/lwip/timeouts.h"
#include "lib/liblwip/include/lwip/dhcp.h"
#include "lib/liblwip/include/netif/etharp.h"
#endif

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace TC6
{

/**************************************************************************************
 * DEFINE
 **************************************************************************************/

#define TC6LwIP_HOSTNAME            "tc6"
#define TC6LwIP_MTU                 (1536u)

/**************************************************************************************
 * GLOBAL CONSTANTS
 **************************************************************************************/

struct TC6ListNode {
  TC6LwIP_t * inst = nullptr;
  struct TC6ListNode * next = nullptr;
};

static TC6ListNode * tc6_lwip_instance_list_head = nullptr;

/**************************************************************************************
 * MODULE INTERNAL FUNCTION DECLARATION
 **************************************************************************************/

static TC6LwIP_t * GetContextNetif(struct netif *intf)
{
  for (TC6ListNode * ptr = tc6_lwip_instance_list_head;
       ptr != nullptr;
       ptr = ptr->next)
  {
    /* Compare memory address to retrieve the right
     * data structure.
     */
    if (&ptr->inst->ip.netint == intf)
      return ptr->inst;
  }
  return nullptr;
}

static TC6LwIP_t *GetContextTC6(TC6_t *pTC6)
{
  for (TC6ListNode * ptr = tc6_lwip_instance_list_head;
       ptr != nullptr;
       ptr = ptr->next)
  {
    /* Compare memory address to retrieve the right
     * data structure.
     */
    if (ptr->inst->tc.tc6 == pTC6)
      return ptr->inst;
  }
  return nullptr;
}

static void OnPlcaStatus(TC6_t *pInst, bool success, uint32_t addr, uint32_t value, void *tag, void *pGlobalTag)
{
  TC6LwIP_t *lw = (TC6LwIP_t *) tag;
  (void) pInst;
  (void) addr;
  (void) pGlobalTag;
  if ((NULL != lw) && (NULL != lw->tc.pStatusCallback))
  {
    bool status = false;
    if (success)
    {
      status = (0u != ((1u << 15) & value));
    }
    lw->tc.pStatusCallback(success, status);
  }
}

static err_t lwIpInit(struct netif *netif);
static err_t lwIpOut(struct netif *netif, struct pbuf *p);

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

TC6_Arduino_10BASE_T1S::TC6_Arduino_10BASE_T1S(TC6_Io & tc6_io)
: _tc6_io{tc6_io}
{
  _lw.io = &tc6_io;
}

TC6_Arduino_10BASE_T1S::~TC6_Arduino_10BASE_T1S()
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

bool TC6_Arduino_10BASE_T1S::begin(IPAddress const ip_addr,
                                   IPAddress const network_mask,
                                   IPAddress const gateway,
                                   MacAddress const mac_addr,
                                   T1SPlcaSettings const t1s_plca_settings,
                                   T1SMacSettings const t1s_mac_settings)
{
  /* Initialize LWIP only once.
   * On RP2040 with a plain Pico (no WiFi / CYW43), the arduino-pico core does
   * NOT call lwip_init() automatically — that only happens on Pico W via the
   * CYW43 WiFi driver.  We must therefore call it ourselves on every platform.
   * The static guard prevents double-initialisation if begin() is called twice. */
  static bool is_lwip_init = false;
  if (!is_lwip_init)
  {
    lwip_init();
    is_lwip_init = true;
  }

  /* Store MAC address to LWIP. */
  memcpy(_lw.ip.mac, mac_addr.data(), sizeof(_lw.ip.mac));

  /* Initialize the TC6 library and pass a global tag. */
  _lw.tc.tc6 = TC6_Init(&_lw);
  if (_lw.tc.tc6 == NULL) {
    return false;
  }

  TC6ListNode * ptr = tc6_lwip_instance_list_head;
  while (ptr != nullptr) ptr = ptr->next;
  ptr = new TC6ListNode;
  ptr->inst = &_lw;
  ptr->next = NULL;
  if (tc6_lwip_instance_list_head == nullptr) {
    tc6_lwip_instance_list_head = ptr;
  }

  /* Initialize TC6 registers. */
  if (!TC6Regs_Init(  _lw.tc.tc6
                    , &_lw
                    , _lw.ip.mac
                    , true /* enable_plca */
                    , t1s_plca_settings.nodeId()
                    , t1s_plca_settings.nodeCount()
                    , t1s_plca_settings.burstCount()
                    , t1s_plca_settings.burstTimer()
                    , t1s_mac_settings.isMacPromiscuousModeEnabled()
                    , t1s_mac_settings.isMacTxCutThroughEnabled()
                    , t1s_mac_settings.isMacRxCutThroughEnabled()))
    return false;

  /* Complete initialization. */
  while (!TC6Regs_GetInitDone(_lw.tc.tc6))
    TC6_Service(_lw.tc.tc6, true);

  /* Assign IP address, network mask and gateway. */
  auto const ip_addr_str = ip_addr.toString();
  auto const network_mask_str = network_mask.toString();
  auto const gateway_str = gateway.toString();

  ip4_addr_t lwip_ip_addr;
  ip4_addr_t lwip_network_mask;
  ip4_addr_t lwip_gateway;

  ipaddr_aton(ip_addr_str.c_str(), &lwip_ip_addr);
  ipaddr_aton(network_mask_str.c_str(), &lwip_network_mask);
  ipaddr_aton(gateway_str.c_str(), &lwip_gateway);

  /* Bring up the interface. Both the bundled lwIP and the arduino-pico core
   * use NO_SYS=1, so ethernet_input is correct on all platforms. */
  if (!netif_add(&_lw.ip.netint, &lwip_ip_addr, &lwip_network_mask, &lwip_gateway, &_lw, lwIpInit, ethernet_input))
    return false;

  netif_set_link_up(&_lw.ip.netint);

  /* Start DHCP if no static IP address was provided (all-zero IP). */
  if (ip4_addr_isany_val(lwip_ip_addr)) {
    dhcp_start(&_lw.ip.netint);
  }

  /* Copy the settings for internal usage. */
  _t1s_plca_settings = t1s_plca_settings;

  return true;
}

void TC6_Arduino_10BASE_T1S::digitalWrite(DIO const dio, bool const value)
{
  if (dio == DIO::A0)
    digitalWrite_A0(value);
  else if (dio == DIO::A1)
    digitalWrite_A1(value);
}

void TC6_Arduino_10BASE_T1S::service()
{
  sys_check_timeouts(); /* LWIP timers - ARP, DHCP, TCP, etc. */

  if (_tc6_io.isInterruptActive())
  {
    if (TC6_Service(_lw.tc.tc6, false))
    {
      _tc6_io.releaseInterrupt();
    }
  } else if (_lw.tc.tc6NeedService)
  {
    _lw.tc.tc6NeedService = false;
    TC6_Service(_lw.tc.tc6, true);
  }

  TC6Regs_CheckTimers();
}

bool TC6_Arduino_10BASE_T1S::getPlcaStatus(TC6LwIP_On_PlcaStatus on_plca_status)
{
  _lw.tc.pStatusCallback = on_plca_status;
  return TC6_ReadRegister(_lw.tc.tc6, 0x0004CA03, true, OnPlcaStatus, &_lw); /* PLCA_status_register.plca_status */
}

IPAddress TC6_Arduino_10BASE_T1S::localIP()
{
  const ip4_addr_t * addr = netif_ip4_addr(&_lw.ip.netint);
  return IPAddress(ip4_addr1(addr), ip4_addr2(addr), ip4_addr3(addr), ip4_addr4(addr));
}

void TC6_Arduino_10BASE_T1S::restartDhcp()
{
  dhcp_stop(&_lw.ip.netint);
  dhcp_start(&_lw.ip.netint);
}

bool TC6_Arduino_10BASE_T1S::enablePlca()
{
  return TC6Regs_SetPlca(_lw.tc.tc6, true, _t1s_plca_settings.nodeId(), _t1s_plca_settings.nodeCount());
}

bool TC6_Arduino_10BASE_T1S::sendWouldBlock()
{
  TC6_RawTxSegment *dummySeg;
  uint8_t const segCount = TC6_GetRawSegments(_lw.tc.tc6, &dummySeg);
  bool const wouldBlock = (0u == segCount);

  return wouldBlock;
}

void TC6_Arduino_10BASE_T1S::digitalWrite_A0(bool const value)
{
  static bool is_dio_a0_enabled = false;
  if (!is_dio_a0_enabled)
  {
    TC6Regs_EnableDio_A0(_lw.tc.tc6);
    is_dio_a0_enabled = true;
  }

  static bool dio_a0_val = false;
  if (value != dio_a0_val)
  {
    TC6Regs_ToggleDio_A0(_lw.tc.tc6);
    dio_a0_val = value;
  }
}

void TC6_Arduino_10BASE_T1S::digitalWrite_A1(bool const value)
{
  static bool is_dio_a1_enabled = false;
  if (!is_dio_a1_enabled)
  {
    TC6Regs_EnableDio_A1(_lw.tc.tc6);
    is_dio_a1_enabled = true;
  }

  static bool dio_a1_val = false;
  if (value != dio_a1_val)
  {
    TC6Regs_ToggleDio_A1(_lw.tc.tc6);
    dio_a1_val = value;
  }
}

/**************************************************************************************
 * LWIP CALLBACKS
 **************************************************************************************/

static err_t lwIpInit(struct netif *netif)
{
  TC6LwIP_t *lw = GetContextNetif(netif);
  netif->output = etharp_output;
  netif->linkoutput = lwIpOut;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
  netif->mtu = TC6LwIP_MTU;
  netif->hwaddr_len = ETHARP_HWADDR_LEN;
  (void) memcpy(netif->name, TC6LwIP_HOSTNAME, 2);
  (void) memcpy(netif->hwaddr, lw->ip.mac, NETIF_MAX_HWADDR_LEN);
  netif_set_up(netif);
  netif_set_default(netif);
  return ERR_OK;
}

static err_t lwIpOut(struct netif *netif, struct pbuf *p)
{
  TC6_RawTxSegment *txSeg = NULL;
  TC6LwIP_t *lw = GetContextNetif(netif);
  struct pbuf *pC = p;
  uint8_t maxSeg;
  uint8_t seg = 0;
  err_t result;
  bool success;
//  TC6_ASSERT(netif && p);
//  TC6_ASSERT(LWIP_TC6_MAGIC == ((TC6LwIP_t*)netif->state)->magic);
  maxSeg = TC6_GetRawSegments(lw->tc.tc6, &txSeg);
  if (maxSeg)
  {
    pbuf_ref(p);
    while (seg < maxSeg)
    {
      txSeg[seg].pEth = (uint8_t *) pC->payload;
      txSeg[seg].segLen = pC->len;
      seg++;
      if (NULL != pC->next)
      {
//        TC6_ASSERT(seg < TC6_TX_ETH_MAX_SEGMENTS);
        pC = pC->next;
      } else
      {
        break;
      }
    }
    success = TC6_SendRawEthernetSegments(
      lw->tc.tc6, txSeg, seg, p->tot_len, 0,
      +[](TC6_t * /* pInst */, const uint8_t * /* pTx */, uint16_t /* len */, void *pTag,
          void * /* pGlobalTag */) -> void
      {
        struct pbuf *p = (struct pbuf *) pTag;
//  TC6_ASSERT(GetContextTC6(pInst));
//  TC6_ASSERT(pTx == p->payload);
//  TC6_ASSERT(len == p->tot_len);
//  TC6_ASSERT(len == p->len);
//  TC6_ASSERT(p->ref);
        pbuf_free(p);
      }, p);
//    TC6_ASSERT(success); /* Must always succeed as TC6_GetRawSegments returned a valid value */
    result = success ? ERR_OK : ERR_IF;
  } else
  {
    result = ERR_WOULDBLOCK;
  }
  return result;
}

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* TC6 */

/**************************************************************************************
 * TC6 CALLBACKS
 **************************************************************************************/

void TC6_CB_OnNeedService(TC6_t *pInst, void *pGlobalTag)
{
  TC6LwIP_t *lw = TC6::GetContextTC6(pInst);
  lw->tc.tc6NeedService = true;
}

uint32_t TC6Regs_CB_GetTicksMs(void)
{
  return millis();
}

bool TC6_CB_OnSpiTransaction(TC6_t *pInst, uint8_t *pTx, uint8_t *pRx, uint16_t len, void *pGlobalTag)
{
  TC6LwIP_t *lw = TC6::GetContextTC6(pInst);
  if (lw == nullptr) {
    return false;
  }
  bool const success = lw->io->spiTransaction(pTx, pRx, len);
  TC6_SpiBufferDone(pInst /* tc6instance */, success /* success */);
  return success;
}

void TC6_CB_OnRxEthernetSlice(TC6_t *pInst, const uint8_t *pRx, uint16_t offset, uint16_t len, void *pGlobalTag)
{
  TC6LwIP_t *lw = TC6::GetContextTC6(pInst);
  bool success = true;
  (void)pInst;
  (void)pGlobalTag;

  /* If this is the start of a new frame (offset==0) and a pbuf is already
   * held from a previous interrupted frame, free it before proceeding.
   * Without this, a reinit mid-frame leaves a permanently stuck pbuf that
   * causes every subsequent RX frame to be written into the wrong buffer
   * and eventually exhausts MEM_SIZE. */
  if (offset == 0 && lw->tc.pbuf != NULL)
  {
    Serial.println("[TC6 RX] Stale pbuf detected at frame start — freeing");
    pbuf_free(lw->tc.pbuf);
    lw->tc.pbuf     = NULL;
    lw->tc.rxLen    = 0;
    lw->tc.rxInvalid = false;
  }
//  TC6_ASSERT(lw->tc.tc6 == pInst);
  if (lw->tc.rxInvalid) {
    success = false;
  }
  if (success && ((offset + len) > TC6LwIP_MTU)) {
//    PRINT("on_rx_slice:packet greater than MTU", (offset + len));
    lw->tc.rxInvalid = true;
    success = false;
  }
  if (success && (NULL == lw->tc.pbuf)) {
    lw->tc.pbuf = pbuf_alloc(PBUF_RAW, TC6LwIP_MTU, PBUF_RAM);
    if (!lw->tc.pbuf) {
      lw->tc.rxInvalid = true;
      success = false;
    }
    if (success && (NULL != lw->tc.pbuf->next)) {
//      PRINT("rx_slice: could not allocate unsegmented memory diff", (lw->tc.pbuf->tot_len - lw->tc.pbuf->len));
      lw->tc.rxInvalid = true;
      pbuf_free(lw->tc.pbuf);
      lw->tc.pbuf = NULL;
      success = false;
    }
  }
  if (success) {
    (void)memcpy(lw->tc.pbuf->payload + offset, pRx, len);
    lw->tc.rxLen += len;
  }
}

void TC6_CB_OnRxEthernetPacket(TC6_t *pInst, bool success, uint16_t len, uint64_t *rxTimestamp, void *pGlobalTag)
{
#define MIN_HEADER_LEN  (42u)
  TC6LwIP_t *lw = TC6::GetContextTC6(pInst);
  (void) pInst;
  (void) rxTimestamp;
  (void) pGlobalTag;
//  TC6_ASSERT(lw->tc.tc6 == pInst);
  bool result = true;
  if (!success || lw->tc.rxInvalid || !lw->tc.pbuf || !lw->tc.rxLen)
  {
    result = false;
  }
  if (result && (lw->tc.rxLen != len))
  {
//        PrintRateLimited("on_rx_eth_ready: size mismatch", 0u);
    result = false;
  }
  if (result && (len < MIN_HEADER_LEN))
  {
//        PrintRateLimited("on_rx_eth_ready: received invalid small packet len", len);
    result = false;
  }
  if (result)
  {
    pbuf_realloc(lw->tc.pbuf, len); /* Shrink pbuf to actual received length. */

    /* Pass the frame to lwIP's ethernet_input() unconditionally.
     *
     * WHY WE REMOVED FilterRxEthernetPacket:
     * ethernet_input() already handles every EtherType internally:
     *   - IPv4 / ARP  → passed to ip4_input() / etharp_input()
     *   - unknown     → goto free_and_return → pbuf_free(p) + ERR_OK
     * Filtering here just caused pbuf leaks for unrecognised frames
     * (the old `else` branch skipped the cleanup block).
     *
     * WHY WE NEVER CALL pbuf_free AFTER input():
     * ethernet_input() ALWAYS returns ERR_OK (every code path either
     * consumes the pbuf or calls pbuf_free internally before returning).
     * Calling pbuf_free again after input() is always a double-free and
     * corrupts the lwIP memp pool, crashing the stack within minutes.
     *
     * The original FIXME "SOMETHING FISHY GOING ON" was exactly this. */
    lw->ip.netint.input(lw->tc.pbuf, &lw->ip.netint);

    /* lwIP owns the pbuf now — clear our pointer. */
    lw->tc.pbuf     = NULL;
    lw->tc.rxLen    = 0;
    lw->tc.rxInvalid = false;
  }
  if (!result)
  {
    /* Early-reject path (bad flags, size mismatch, etc.).
     * lwIP never received this pbuf, so we must free it ourselves. */
    if (NULL != lw->tc.pbuf)
    {
      pbuf_free(lw->tc.pbuf);
      lw->tc.pbuf = NULL;
    }
    lw->tc.rxLen    = 0;
    lw->tc.rxInvalid = false;
  }
}

#define PRINT(...)

/* Reinit counter — incremented every time TC6Regs_Reinit() is triggered.
 * Visible in the [MEM] diagnostic line in main.cpp. */
uint32_t g_tc6_reinit_count = 0;

void TC6_CB_OnError(TC6_t *pInst, TC6_Error_t err, void *pGlobalTag)
{
  bool reinit = false;
  switch (err)
  {
    case TC6Error_Succeeded:        /* normal, no print */ break;
    case TC6Error_NoHardware:
      Serial.println("[TC6 ERR] NoHardware — MACPHY not responding");
      reinit = true; break;
    case TC6Error_UnexpectedSv:
      Serial.println("[TC6 ERR] UnexpectedStartValid"); break;
    case TC6Error_UnexpectedDvEv:
      Serial.println("[TC6 ERR] UnexpectedDataValid/EndValid"); break;
    case TC6Error_BadChecksum:
      Serial.println("[TC6 ERR] BadChecksum");
      reinit = true; break;
    case TC6Error_UnexpectedCtrl:
      Serial.println("[TC6 ERR] UnexpectedCtrlPacket");
      reinit = true; break;
    case TC6Error_BadTxData:
      Serial.println("[TC6 ERR] BadTxData (Header Bad Flag)");
      reinit = true; break;
    case TC6Error_SyncLost:
      Serial.println("[TC6 ERR] SyncLost — reinitialising");
      reinit = true; break;
    case TC6Error_SpiError:
      Serial.println("[TC6 ERR] SpiError — reinitialising");
      reinit = true; break;
    case TC6Error_ControlTxFail:
      Serial.println("[TC6 ERR] ControlTxFail"); break;
    default:
      Serial.println("[TC6 ERR] Unknown error"); break;
  }
  if (reinit)
  {
    g_tc6_reinit_count++;
    TC6Regs_Reinit(pInst);
  }
}

void TC6Regs_CB_OnEvent(TC6_t *pInst, TC6Regs_Event_t event, void *pTag)
{
  bool reinit = false;
  switch (event)
  {
    case TC6Regs_Event_UnknownError:
      Serial.println("[TC6 EVT] UnknownError"); break;
    case TC6Regs_Event_Transmit_Protocol_Error:
      Serial.println("[TC6 EVT] Transmit_Protocol_Error"); break;
    case TC6Regs_Event_Transmit_Buffer_Overflow_Error:
      Serial.println("[TC6 EVT] TX_Buffer_Overflow"); break;
    case TC6Regs_Event_Transmit_Buffer_Underflow_Error:
      Serial.println("[TC6 EVT] TX_Buffer_Underflow"); break;
    case TC6Regs_Event_Receive_Buffer_Overflow_Error:
      Serial.println("[TC6 EVT] RX_Buffer_Overflow"); break;
    case TC6Regs_Event_Loss_of_Framing_Error:
      Serial.println("[TC6 EVT] Loss_of_Framing — reinitialising");
      reinit = true; break;
    case TC6Regs_Event_Header_Error:
      Serial.println("[TC6 EVT] Header_Error"); break;
    case TC6Regs_Event_Reset_Complete:
      Serial.println("[TC6 EVT] Reset_Complete"); break;
    case TC6Regs_Event_PHY_Interrupt:
      /* frequent — omit to avoid flooding serial */ break;
    case TC6Regs_Event_Transmit_Frame_Check_Sequence_Error:
      Serial.println("[TC6 EVT] TX_FCS_Error"); break;
    case TC6Regs_Event_Control_Data_Protection_Error:
      Serial.println("[TC6 EVT] Control_Data_Protection_Error"); break;
    case TC6Regs_Event_RX_Non_Recoverable_Error:
      Serial.println("[TC6 EVT] RX_Non_Recoverable — reinitialising");
      reinit = true; break;
    case TC6Regs_Event_TX_Non_Recoverable_Error:
      Serial.println("[TC6 EVT] TX_Non_Recoverable — reinitialising");
      reinit = true; break;
    case TC6Regs_Event_FSM_State_Error:
      Serial.println("[TC6 EVT] FSM_State_Error"); break;
    case TC6Regs_Event_SRAM_ECC_Error:
      Serial.println("[TC6 EVT] SRAM_ECC_Error"); break;
    case TC6Regs_Event_Undervoltage:
      Serial.println("[TC6 EVT] UNDERVOLTAGE — check power supply!"); break;
    case TC6Regs_Event_Internal_Bus_Error:
      Serial.println("[TC6 EVT] Internal_Bus_Error"); break;
    case TC6Regs_Event_Chip_Error:
      Serial.println("[TC6 EVT] Chip_Error — PHY not trimmed"); break;
    case TC6Regs_Event_Unsupported_Hardware:
      Serial.println("[TC6 EVT] Unsupported_Hardware"); break;
    default: break;
  }
  if (reinit)
  {
    g_tc6_reinit_count++;
    TC6Regs_Reinit(pInst);
  }
}
