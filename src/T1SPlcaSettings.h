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

#include <stdint.h>

#include <Print.h>
#include <Printable.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class T1SPlcaSettings
#if defined(ARDUINO_ARCH_AVR)
  : public Printable
#else
  : public arduino::Printable
#endif
{
private:
  uint8_t _node_id;
  uint8_t _node_count;
  uint8_t _burst_count;
  uint8_t _burst_timer;

public:
  static uint8_t const DEFAULT_NODE_ID     =   0;
  static uint8_t const DEFAULT_NODE_COUNT  =   8;
  static uint8_t const DEFAULT_BURST_COUNT =   0;
  static uint8_t const DEFAULT_BURST_TIMER = 128;


  T1SPlcaSettings() : T1SPlcaSettings(DEFAULT_NODE_ID, DEFAULT_NODE_COUNT, DEFAULT_BURST_COUNT, DEFAULT_BURST_TIMER) { }
  T1SPlcaSettings(uint8_t const node_id) : T1SPlcaSettings(node_id, DEFAULT_NODE_COUNT, DEFAULT_BURST_COUNT, DEFAULT_BURST_TIMER) { }
  T1SPlcaSettings(uint8_t const node_id,
                  uint8_t const node_count,
                  uint8_t const burst_count,
                  uint8_t const burst_timer);


  virtual size_t printTo(Print & p) const override;


  uint8_t node_id()     const { return _node_id; }
  uint8_t node_count()  const { return _node_count; }
  uint8_t burst_count() const { return _burst_count; }
  uint8_t burst_timer() const { return _burst_timer; }
};
