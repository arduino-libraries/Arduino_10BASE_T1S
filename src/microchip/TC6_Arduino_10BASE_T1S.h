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

#include "../Arduino_10BASE_T1S_PHY_Interface.h"

#include <stdint.h>

#include "lib/liblwip/include/lwip/netif.h"

#include "microchip/lib/libtc6/inc/tc6.h"

#include "TC6_Io.h"

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

typedef void (*TC6LwIP_On_PlcaStatus)(bool success, bool plcaStatus);

typedef struct
{
  TC6_t *tc6;
  struct pbuf *pbuf;
  TC6LwIP_On_PlcaStatus pStatusCallback;
  uint16_t rxLen;
  bool rxInvalid;
  bool tc6NeedService;
} TC6Lib_t;

typedef struct
{
  char ipAddr[16];
  struct netif netint;
  uint8_t mac[6];
} LwIp_t;

typedef struct
{
  TC6Lib_t tc;
  LwIp_t ip;
  TC6::TC6_Io * io;
} TC6LwIP_t;

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace TC6
{

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

enum class DIO { A0, A1 };

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6_Arduino_10BASE_T1S : public Arduino_10BASE_T1S_PHY_Interface
{
public:
  /**
   * @class TC6_Arduino_10BASE_T1S
   * @brief Arduino 10BASE-T1S PHY interface implementation using TC6.
   *
   * This class provides the implementation of the Arduino_10BASE_T1S_PHY_Interface
   * using the TC6 hardware abstraction layer for 10BASE-T1S communication.
   */
  TC6_Arduino_10BASE_T1S(TC6_Io & tc6_io);

  virtual ~TC6_Arduino_10BASE_T1S();

  /**
   * @brief Initializes the TC6 Arduino 10BASE-T1S PHY interface with the specified network settings.
   *
   * This method configures the TC6 interface with the provided IP address,
   * network mask, gateway, MAC address, and PLCA settings.
   *
   * @param ip_addr The IP address to assign to the interface.
   * @param network_mask The network mask to use.
   * @param gateway The gateway IP address.
   * @param mac_addr The MAC address to assign to the interface.
   * @param t1s_plca_settings The PLCA settings to use.
   * @param t1s_mac_settings The MAC settings to use.
   * @return Returns true if the initialization was successful, false otherwise.
   */
  virtual bool begin(IPAddress const ip_addr,
                     IPAddress const network_mask,
                     IPAddress const gateway,
                     MacAddress const mac_addr,
                     T1SPlcaSettings const t1s_plca_settings,
                     T1SMacSettings const t1s_mac_settings) override;


  virtual void service() override;
  
  void digitalWrite(DIO const dio, bool const value);

  /**
   * @brief Gets the PLCA status.
   *
   * This method retrieves the current PLCA status from the TC6 hardware.
   *
   * @param on_plca_status Callback function to handle the PLCA status.
   * @return bool Returns true if the PLCA status was successfully retrieved, false otherwise.
   */
  bool getPlcaStatus(TC6LwIP_On_PlcaStatus on_plca_status);
  /**
   * @brief Enables PLCA (Physical Layer Collision Avoidance).
   *
   * This method enables the PLCA functionality on the TC6 hardware.
   *
   * @return bool Returns true if PLCA was successfully enabled, false otherwise.
   */
  bool enablePlca();

  /**
   * @brief Checks if sending data would block.
   *
   * This method checks if the TC6 hardware is currently able to send data
   * without blocking.
   *
   * @return bool Returns true if sending data would block, false otherwise.
   */
  bool sendWouldBlock();

private:
  TC6_Io & _tc6_io;
  TC6LwIP_t _lw;
  T1SPlcaSettings _t1s_plca_settings;

  void digitalWrite_A0(bool const value);
  void digitalWrite_A1(bool const value);
};

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* TC6 */
