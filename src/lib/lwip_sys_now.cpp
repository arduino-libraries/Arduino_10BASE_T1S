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

/* On RP2040 (arduino-pico) the core already provides sys_now() as part of
 * its own lwIP port.  Compiling this file on that platform would cause a
 * duplicate-symbol link error, so we skip the entire translation unit. */
#if !defined(ARDUINO_ARCH_RP2040)

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>

#include "liblwip/include/lwip/opt.h"
#include "liblwip/arch/cc.h"

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/

extern "C" u32_t sys_now(void)
{
  return millis();
}

extern "C" u32_t t1s_sys_now(void)
{
  return millis();
}

extern "C" void t1s_lwip_itoa(char * result, size_t bufsize, int number)
{
  if ((result == nullptr) || (bufsize == 0))
    return;

  snprintf(result, bufsize, "%d", number);
}

extern "C" u16_t t1s_lwip_htons(u16_t x)
{
  return (u16_t)((x << 8) | (x >> 8));
}

extern "C" u32_t t1s_lwip_htonl(u32_t x)
{
  return ((x & 0x000000FFUL) << 24)
       | ((x & 0x0000FF00UL) << 8)
       | ((x & 0x00FF0000UL) >> 8)
       | ((x & 0xFF000000UL) >> 24);
}

#endif /* !ARDUINO_ARCH_RP2040 */
