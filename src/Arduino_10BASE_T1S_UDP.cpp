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

#include "Arduino_10BASE_T1S_UDP.h"

#if LWIP_IGMP
#if defined(ARDUINO_ARCH_RP2040)
#include <lwip/igmp.h>
#else
#include "lib/liblwip/include/lwip/igmp.h"
#endif
#endif

/**************************************************************************************
 * MODULE INTERNAL FUNCTION DECLARATION
 **************************************************************************************/

static void lwIp_udp_raw_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port);

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

Arduino_10BASE_T1S_UDP::Arduino_10BASE_T1S_UDP()
: _udp_pcb{nullptr}
, _send_to_ip{0,0,0,0}
, _send_to_port{0}
, _rx_pkt{nullptr}
{

}

Arduino_10BASE_T1S_UDP::~Arduino_10BASE_T1S_UDP()
{
  stop();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

uint8_t Arduino_10BASE_T1S_UDP::begin(uint16_t port)
{
  /* Create a UDP PCB (if none exists yet). */
  if (!_udp_pcb)
    _udp_pcb = udp_new();

  /* Bind specified port to all local interfaces. */
  err_t const err = udp_bind(_udp_pcb, IP_ADDR_ANY, port);
  if (err != ERR_OK)
    return 0;

  /* Set a reception callback to be called upon the arrival of a UDP package. */
  udp_recv(_udp_pcb , lwIp_udp_raw_recv, this);

  return 1;
}

void Arduino_10BASE_T1S_UDP::stop()
{
  if (_udp_pcb != nullptr)
  {
    udp_disconnect(_udp_pcb);
    udp_remove(_udp_pcb);
    _udp_pcb = nullptr;
  }
}

int Arduino_10BASE_T1S_UDP::beginPacket(IPAddress ip, uint16_t port)
{
  if (_udp_pcb == nullptr)
    return 0;

  _send_to_ip = ip;
  _send_to_port = port;

  /* Make sure that the transmit data buffer is empty. */
  _tx_data.clear();

  return 1;
}

int Arduino_10BASE_T1S_UDP::beginPacket(const char *host, uint16_t port)
{
  /* TODO */
  return 0;
}

int Arduino_10BASE_T1S_UDP::endPacket()
{
  if (_udp_pcb == nullptr)
    return 0;

  /* Convert to IP address required for LWIP. */
  ip_addr_t ipaddr;
  IP_ADDR4(&ipaddr, _send_to_ip[0], _send_to_ip[1], _send_to_ip[2], _send_to_ip[3]);

  /* Allocate pbuf structure. */
  struct pbuf * p = pbuf_alloc(PBUF_TRANSPORT, _tx_data.size(), PBUF_RAM);
  if (!p)
    return 0;

  /* Copy data from transmit buffer over. */
  err_t err = pbuf_take(p, _tx_data.data(), _tx_data.size());
  if (err != ERR_OK)
  {
    pbuf_free(p);
    return -1;
  }

  /* Empty our transmit buffer. */
  _tx_data.clear();

  /* Send UDP packet. */
  err = udp_sendto(_udp_pcb, p, &ipaddr, _send_to_port);
  /* Always free our reference — lwIP has ref'd its own copy if it needed one. */
  pbuf_free(p);
  if (err != ERR_OK)
    return -1;

  return 1;
}

size_t Arduino_10BASE_T1S_UDP::write(uint8_t data)
{
  _tx_data.push_back(data);
  return 1;
}

size_t Arduino_10BASE_T1S_UDP::write(const uint8_t * buffer, size_t size)
{
  std::copy(buffer, buffer + size, std::back_inserter(_tx_data));
  return size;
}

int Arduino_10BASE_T1S_UDP::parsePacket()
{
  if (_rx_pkt_list.size())
  {
    /* Discard UdpRxPacket object previously held by _rx_pkt
     * and replace it with the new one.
     */
    _rx_pkt = _rx_pkt_list.front();
    _rx_pkt_list.pop_front();
    return _rx_pkt->totalSize();
  }
  else
  {
    /* Otherwise ensure that _rx_pkt definitely
     * does not hold any UdpRxPacket object anymore.
     */
    _rx_pkt.reset();
    return 0;
  }
}

int Arduino_10BASE_T1S_UDP::available()
{
  if (_rx_pkt)
    return _rx_pkt->available();
  else
    return 0;
}

int Arduino_10BASE_T1S_UDP::read()
{
  if (_rx_pkt)
   return _rx_pkt->read();
  else
    return -1;
}

int Arduino_10BASE_T1S_UDP::read(unsigned char* buffer, size_t len)
{
  if (_rx_pkt)
    return _rx_pkt->read(buffer, len);
  else
    return -1;
}

int Arduino_10BASE_T1S_UDP::read(char* buffer, size_t len)
{
  if (_rx_pkt)
    return _rx_pkt->read(buffer, len);
  else
    return -1;
}

int Arduino_10BASE_T1S_UDP::peek()
{
  if (_rx_pkt)
    return _rx_pkt->peek();
  else
    return -1;
}

void Arduino_10BASE_T1S_UDP::flush()
{
  /* Delete UdpRxPacket object held by _rx_pkt. */
  if (_rx_pkt)
    _rx_pkt.reset();
}

IPAddress Arduino_10BASE_T1S_UDP::remoteIP()
{
  if (_rx_pkt)
    return _rx_pkt->remoteIP();
  else
    return IPAddress();
}

uint16_t Arduino_10BASE_T1S_UDP::remotePort()
{
  if (_rx_pkt)
    return _rx_pkt->remotePort();
  else
    return 0;
}

uint8_t Arduino_10BASE_T1S_UDP::beginMulticast(IPAddress multicast_ip, uint16_t port)
{
  /* Bind the local port first. */
  if (!begin(port))
    return 0;

  /* Join the IGMP group so the netif passes frames destined for this address. */
  if (!joinMulticast(multicast_ip))
  {
    stop();
    return 0;
  }

  return 1;
}

bool Arduino_10BASE_T1S_UDP::joinMulticast(IPAddress const group_ip)
{
#if LWIP_IGMP
  ip4_addr_t group;
  IP4_ADDR(&group, group_ip[0], group_ip[1], group_ip[2], group_ip[3]);
  return (igmp_joingroup(IP4_ADDR_ANY4, &group) == ERR_OK);
#else
  (void)group_ip;
  return false;
#endif
}

bool Arduino_10BASE_T1S_UDP::leaveMulticast(IPAddress const group_ip)
{
#if LWIP_IGMP
  ip4_addr_t group;
  IP4_ADDR(&group, group_ip[0], group_ip[1], group_ip[2], group_ip[3]);
  return (igmp_leavegroup(IP4_ADDR_ANY4, &group) == ERR_OK);
#else
  (void)group_ip;
  return false;
#endif
}

void Arduino_10BASE_T1S_UDP::onUdpRawRecv(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
  /* Obtain remote port and remote IP. */
  auto const remote_ip = IPAddress(
    ip4_addr1(addr),
    ip4_addr2(addr),
    ip4_addr3(addr),
    ip4_addr4(addr));
  auto const remote_port = port;

  /* Flatten the pbuf chain into a contiguous buffer.
   * p->len  is only the first segment; p->tot_len is the full datagram.
   * Using p->payload / p->len alone silently truncates chained pbufs. */
  size_t const total_len = p->tot_len;
  std::vector<uint8_t> flat(total_len);
  pbuf_copy_partial(p, flat.data(), (u16_t)total_len, 0);

  auto const rx_pkt = std::make_shared<UdpRxPacket>(
    remote_ip,
    remote_port,
    flat.data(),
    total_len);

  // drop the oldest packet if the list is full
  if(_rx_pkt_list.size() > _rx_pkt_list_size) {
    _rx_pkt_list.pop_front();
  }

  _rx_pkt_list.push_back(rx_pkt);
  /* Free pbuf */
  pbuf_free(p);
}

/**************************************************************************************
 * LWIP CALLBACKS
 **************************************************************************************/

void lwIp_udp_raw_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
  Arduino_10BASE_T1S_UDP * this_ptr = (Arduino_10BASE_T1S_UDP * )arg;
  this_ptr->onUdpRawRecv(pcb, p, addr, port);
}
