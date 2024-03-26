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

/**************************************************************************************
 * MODULE INTERNAL FUNCTION DECLARATION
 **************************************************************************************/

static void lwIp_udp_raw_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port);

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

Arduino_10BASE_T1S_UDP::Arduino_10BASE_T1S_UDP()
: _udp_pcb{nullptr}
, _remote_ip{0,0,0,0}
, _remote_port{0}
, _send_to_ip{0,0,0,0}
, _send_to_port{0}
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
    return -1;

  /* Empty our transmit buffer. */
  _tx_data.clear();

  /* Send UDP packet. */
  err = udp_sendto(_udp_pcb, p, &ipaddr, _send_to_port);
  if (err != ERR_OK)
    return -1;

  /* Free pbuf */
  pbuf_free(p);

  return 1;
}

size_t Arduino_10BASE_T1S_UDP::write(uint8_t data)
{
  _tx_data.push_back(data);
  return 1;
}

size_t Arduino_10BASE_T1S_UDP::write(const uint8_t * buffer, size_t size)
{
  _tx_data.reserve(_tx_data.size() + size);
  std::copy(buffer, buffer + size, std::back_inserter(_tx_data));
  return size;
}

int Arduino_10BASE_T1S_UDP::parsePacket()
{
  return available();
}

int Arduino_10BASE_T1S_UDP::available()
{
  return _rx_data.size();
}

int Arduino_10BASE_T1S_UDP::read()
{
  uint8_t const data = _rx_data.front();
  _rx_data.pop_front();
  return data;
}

int Arduino_10BASE_T1S_UDP::read(unsigned char* buffer, size_t len)
{
  size_t bytes_read = 0;
  for (; bytes_read < len && !_rx_data.empty(); bytes_read++)
  {
    buffer[bytes_read] = _rx_data.front();
    _rx_data.pop_front();
  }
  return bytes_read;
}

int Arduino_10BASE_T1S_UDP::read(char* buffer, size_t len)
{
  return read((unsigned char*)buffer, len);
}

int Arduino_10BASE_T1S_UDP::peek()
{
  return _rx_data.front();
}

void Arduino_10BASE_T1S_UDP::flush()
{
  /* Nothing to be done. */
}

IPAddress Arduino_10BASE_T1S_UDP::remoteIP()
{
  return _remote_ip;
}

uint16_t Arduino_10BASE_T1S_UDP::remotePort()
{
  return _remote_port;
}

void Arduino_10BASE_T1S_UDP::onUdpRawRecv(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
  /* Obtain remote port and remote IP. */
  _remote_ip = IPAddress(ip4_addr1(addr),
                         ip4_addr2(addr),
                         ip4_addr3(addr),
                         ip4_addr4(addr));
  _remote_port = port;

  /* Copy data into buffer. */
  std::copy((uint8_t *)p->payload,
            (uint8_t *)p->payload + p->len,
            std::back_inserter(_rx_data));

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