/*
 * This file is part of the Arduino_10BASE_T1S library.
 * Copyright (c) 2023 Arduino SA.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "../Arduino_10BASE_T1S_UDP.h"

#include <cstdint>

#include <memory>

#include <IPAddress.h>

#include "../MacAddress.h"
#include "../T1SMacSettings.h"
#include "../T1SPlcaSettings.h"

#include "TC6_Io_Generic.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6 : public Arduino_10BASE_T1S_UDP
{
public:
  TC6(std::shared_ptr<TC6_Io_Base> const tc6_io);
  ~TC6();


  bool begin(IPAddress const ip_addr,
             T1SPlcaSettings const t1s_plca_settings,
             T1SMacSettings const t1s_mac_settings);

  void service();

  typedef void (*OnPlcaStatusFunc)(bool success, bool plcaStatus);
  bool getPlcaStatus(OnPlcaStatusFunc on_plca_status);

  bool sendWouldBlock();

  MacAddress getMacAddr();


private:
  std::shared_ptr<TC6_Io_Base> const _tc6_io;
  int8_t _idx;
};
