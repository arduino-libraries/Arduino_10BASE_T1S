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

#include <cstdint>

#include <array>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6
{
public:
  TC6();
  ~TC6();


  bool begin(uint8_t const ip[4],
             bool const enable_plca,
             uint8_t const node_id,
             uint8_t const node_count,
             uint8_t const burst_count,
             uint8_t const burst_timer,
             bool const mac_promiscuous_mode,
             bool const mac_tx_cut_through,
             bool const mac_rx_cut_through);

  void service();

  typedef void (*OnPlcaStatusFunc)(bool success, bool plcaStatus);
  bool getPlcaStatus(OnPlcaStatusFunc on_plca_status);

  bool sendWouldBlock();

  typedef std::array<uint8_t, 6> MACAddr;
  MACAddr getMacAddr();


private:
  int8_t _idx;
};
