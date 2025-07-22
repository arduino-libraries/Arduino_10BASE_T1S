/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <list>
#include <deque>
#include <vector>
#include <memory>

#include <api/Udp.h>
#include <api/IPAddress.h>

#include "lib/liblwip/include/lwip/udp.h"
#include "lib/liblwip/include/lwip/ip_addr.h"

#include "MacAddress.h"
#include "T1SMacSettings.h"
#include "T1SPlcaSettings.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class Arduino_10BASE_T1S_UDP : public UDP
{
public:
  /**
   * @class Arduino_10BASE_T1S_UDP
   * @brief UDP communication class for Arduino 10BASE-T1S library.
   */
           Arduino_10BASE_T1S_UDP();
  virtual ~Arduino_10BASE_T1S_UDP();


  /* arduino:UDP */
  /**
   * @brief Initializes the UDP instance to listen on the specified port.
   *
   * This method sets up the UDP protocol to begin listening for incoming packets
   * on the given port. It should be called before attempting to send or receive
   * UDP packets.
   *
   * @param port The local port to listen on.
   * @return Returns 1 if successful, 0 if there was an error.
   */
  virtual uint8_t begin(uint16_t port) override;


  /**
   * @brief Stops the UDP connection and releases any associated resources.
   *
   * This method overrides the base class implementation to properly close
   * the UDP socket and perform necessary cleanup operations.
   */
  virtual void stop() override;

  /**
   * @brief Begins the construction of a UDP packet to the specified IP address and port.
   *
   * Initializes the UDP packet buffer and prepares it to send data to the given destination.
   *
   * @param ip The destination IP address.
   * @param port The destination port number.
   * @return int Returns 1 on success, 0 on failure.
   */
  virtual int beginPacket(IPAddress ip, uint16_t port) override;

  /**
   * @brief Begins the construction of a UDP packet to the specified host and port.
   *
   * Initializes the UDP packet buffer and prepares it to send data to the given destination.
   *
   * @param host The destination host name or IP address as a string.
   * @param port The destination port number.
   * @return int Returns 1 on success, 0 on failure.
   */
  virtual int beginPacket(const char *host, uint16_t port) override;

  /**
   * @brief Finishes the construction of a UDP packet and sends it.
   *
   * This method finalizes the UDP packet and transmits it to the previously specified
   * destination IP address and port.
   *
   * @return int Returns 1 on success, 0 on failure.
   */
  virtual int endPacket() override;
  
  /**
   * @brief Sends a single byte of data in the current UDP packet.
   *
   * This method appends a single byte to the current UDP packet buffer.
   *
   * @param data The byte of data to send.
   * @return size_t Returns the number of bytes written, which is always 1 for a single byte.
   */
  virtual size_t write(uint8_t data) override;
  
  /**
   * @brief Sends a buffer of data in the current UDP packet.
   *
   * This method appends a specified number of bytes from the provided buffer to the
   * current UDP packet buffer.
   *
   * @param buffer Pointer to the data buffer to send.
   * @param size The number of bytes to write from the buffer.
   * @return size_t Returns the number of bytes written, which may be less than size if an error occurs.
   */
  virtual size_t write(const uint8_t * buffer, size_t size) override;

  /**
   * @brief Sends a string in the current UDP packet.
   *
   * This method appends a null-terminated string to the current UDP packet buffer.
   *
   * @param str Pointer to the null-terminated string to send.
   * @return size_t Returns the number of bytes written, including the null terminator.
   */
  virtual int parsePacket() override;
  
  /**
   * @brief Checks if there are any incoming UDP packets available to read.
   *
   * This method checks the internal buffer for any received UDP packets.
   *
   * @return int Returns the number of available bytes in the current packet, or 0 if no packets are available.
   */
  virtual int available() override;
  
  /**
   * @brief Reads a single byte from the current UDP packet.
   *
   * This method retrieves the next byte from the current UDP packet buffer.
   *
   * @return int Returns the byte read, or -1 if no data is available.
   */
  virtual int read() override;
  
  /**
   * @brief Reads a specified number of bytes from the current UDP packet into a buffer.
   *
   * This method reads data from the current UDP packet into the provided buffer.
   *
   * @param buffer Pointer to the buffer where the data will be stored.
   * @param len The number of bytes to read into the buffer.
   * @return int Returns the number of bytes read, which may be less than len if not enough data is available.
   */
  virtual int read(unsigned char* buffer, size_t len) override;
  
  /**
   * @brief Reads a specified number of bytes from the current UDP packet into a character buffer.
   *
   * This method reads data from the current UDP packet into the provided character buffer.
   *
   * @param buffer Pointer to the character buffer where the data will be stored.
   * @param len The number of bytes to read into the buffer.
   * @return int Returns the number of bytes read, which may be less than len if not enough data is available.
   */
  virtual int read(char* buffer, size_t len) override;
  
  /**
   * @brief Peeks at the next byte in the current UDP packet without removing it from the buffer.
   *
   * This method retrieves the next byte from the current UDP packet buffer without consuming it.
   *
   * @return int Returns the next byte, or -1 if no data is available.
   */
  virtual int peek() override;
  
  /**
   * @brief Flushes the current UDP packet buffer.
   *
   * This method clears the current UDP packet buffer, discarding any unsent data.
   */
  virtual void flush() override;

  /**
   * @brief Returns the IP address of the remote host that sent the last received packet.
   *
   * This method retrieves the IP address of the sender of the last received UDP packet.
   *
   * @return IPAddress Returns the IP address of the remote host.
   */
  virtual IPAddress remoteIP() override;
  
  /**
   * @brief Returns the port number of the remote host that sent the last received packet.
   *
   * This method retrieves the port number of the sender of the last received UDP packet.
   *
   * @return uint16_t Returns the port number of the remote host.
   */
  virtual uint16_t remotePort() override;

  /* This function MUST not be called from the user of this library,
   * it's used for internal purposes only.
   */
  void onUdpRawRecv(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port);
  void bufferSize(int size) {
    _rx_pkt_list_size = size;
  }

private:
  /* LWIP */
  struct udp_pcb * _udp_pcb;

  /* UDP TRANSMISSION */
  IPAddress _send_to_ip;
  uint16_t _send_to_port;
  std::vector<uint8_t> _tx_data;
  int _rx_pkt_list_size = 10;
  /* UDP RECEPTION */
  class UdpRxPacket
  {
  private:
    IPAddress const _remote_ip;
    uint16_t const _remote_port;
    size_t const _rx_data_len;
    std::deque<uint8_t> _rx_data;

  public:
    UdpRxPacket(
      IPAddress const remote_ip,
      uint16_t const remote_port,
      uint8_t const * p_data,
      size_t const data_len)
      : _remote_ip(remote_ip)
      , _remote_port(remote_port)
      , _rx_data_len(data_len)
      , _rx_data(p_data, p_data + data_len)
    {
    }

    typedef std::shared_ptr<UdpRxPacket> SharedPtr;

    IPAddress remoteIP() const { return _remote_ip; }
    uint16_t remotePort() const { return _remote_port; }
    size_t totalSize() const { return _rx_data_len; }

    int available()
    {
      return _rx_data.size();
    }

    int read()
    {
      uint8_t const data = _rx_data.front();
      _rx_data.pop_front();
      return data;
    }

    int read(unsigned char* buffer, size_t len)
    {
      size_t bytes_read = 0;
      for (; bytes_read < len && !_rx_data.empty(); bytes_read++)
      {
        buffer[bytes_read] = _rx_data.front();
        _rx_data.pop_front();
      }
      return bytes_read;
    }

    int read(char* buffer, size_t len)
    {
      return read((unsigned char*)buffer, len);
    }

    int peek()
    {
      return _rx_data.front();
    }
  };
  std::list<UdpRxPacket::SharedPtr> _rx_pkt_list;
  UdpRxPacket::SharedPtr _rx_pkt;
};
