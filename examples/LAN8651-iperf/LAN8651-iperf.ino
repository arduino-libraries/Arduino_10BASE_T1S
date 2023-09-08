/*
 * This example has been tested with Arduino Nano 33 IoT and with
 * Mikroe Two-Wire ETH Click board (MicroChip LAN8651). For further
 * information take a look at the README.
 *
 * Author:
 *  Alexander Entinger
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_10BASE_T1S.h>

#include <SPI.h>
#include <Wire.h>

#include "udp_perf_client.h"

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint8_t const T1S_PLCA_NODE_ID     = 1;
static uint8_t const T1S_PLCA_NODE_COUNT  = 8;
static uint8_t const T1S_PLCA_BURST_COUNT = 0;
static uint8_t const T1S_PLCA_BURST_TIMER = 0x80;
static bool    const MAC_PROMISCUOUS_MODE = false;
static bool    const MAC_TX_CUT_THROUGH   = false;
static bool    const MAC_RX_CUT_THROUGH   = false;

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID, T1S_PLCA_NODE_COUNT, T1S_PLCA_BURST_COUNT, T1S_PLCA_BURST_TIMER};
static T1SMacSettings const t1s_mac_settings{MAC_PROMISCUOUS_MODE, MAC_TX_CUT_THROUGH, MAC_RX_CUT_THROUGH};

#if defined(ARDUINO_SAMD_NANO_33_IOT)
static int const CS_PIN    = 10;
static int const RESET_PIN =  9;
static int const IRQ_PIN   =  2;
#else
# error "No pins defined for your board"
#endif

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

auto const tc6_io = std::make_shared<TC6_Io_Generic>
  ( SPI
  , Wire
  , CS_PIN
  , RESET_PIN
  , IRQ_PIN);
auto const tc6_inst = std::make_shared<TC6>(tc6_io);

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);


  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN),
                  []() { tc6_io->onInterrupt(); },
                  FALLING);

  if (!tc6_inst->begin(  ip_addr
                       , network_mask
                       , gateway
                       , t1s_plca_settings
                       , t1s_mac_settings))
  {
    Serial.println("'TC6::begin(...)' failed.");
    return;
  }

  Serial.print("IP\t");
  Serial.println(ip_addr);
  Serial.println(tc6_inst->getMacAddr());
  Serial.println(t1s_plca_settings);
  Serial.println(t1s_mac_settings);

  iperf_init(tc6_inst);
  iperf_print_app_header();
  iperf_start_application();
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  tc6_inst->service();

  iperf_service();

  static unsigned long prev_beacon_check = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    tc6_inst->getPlcaStatus(OnPlcaStatus);
  }
}


static void OnPlcaStatus(bool success, bool plcaStatus)
{
  if (!success)
  {
    Serial.println("PLCA status register read failed");
    return;
  }

  if (plcaStatus)
    Serial.println("PLCA Mode active");
  else
    Serial.println("CSMA/CD fallback");
}
