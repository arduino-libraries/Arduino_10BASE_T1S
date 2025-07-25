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

#include <IPAddress.h> // api/IPAddress.h

#include "MacAddress.h"
#include "T1SMacSettings.h"
#include "T1SPlcaSettings.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class Arduino_10BASE_T1S_PHY_Interface
{
public:
  virtual ~Arduino_10BASE_T1S_PHY_Interface() { }

  /**
   * @brief Initializes the PHY interface with the specified network settings.
   *
   * This method configures the PHY interface with the provided IP address,
   * network mask, gateway, MAC address, and PLCA settings.
   *
   * @param ip_addr The IP address to assign to the interface.
   * @param network_mask The network mask to use.
   * @param gateway The gateway IP address.
   * @param mac_addr The MAC address to assign to the interface.
   * @param t1s_plca_settings The PLCA settings to use.
   * @return Returns true if the initialization was successful, false otherwise.
   */
  virtual bool begin(IPAddress const ip_addr,
                     IPAddress const network_mask,
                     IPAddress const gateway,
                     MacAddress const mac_addr,
                     T1SPlcaSettings const t1s_plca_settings,
                     T1SMacSettings const t1s_mac_settings) = 0;

  virtual void service() = 0;
};
