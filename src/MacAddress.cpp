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

#include "MacAddress.h"

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

void get_unique_chip_id_3(uint8_t * uid);

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

MacAddress::MacAddress(uint8_t const * mac)
{
  if (mac)
    memcpy(_data, mac, MAC_ADDRESS_NUM_BYTES);
  else
    memset(_data, 0, MAC_ADDRESS_NUM_BYTES);
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

MacAddress MacAddress::create_from_uid()
{
  /* The generated MAC address is comprised from Arduino's
   * Organisationally Unique Identifier (OUI) ...
   */
  uint8_t mac_addr[MAC_ADDRESS_NUM_BYTES] = {0xA8, 0x61, 0x0A, 0, 0, 0};
  /* ... as well as from a part of the unique id from
   * each MCU.
   */
  static uint8_t const MAC_NIC_SPECIFIC_OFFSET = 3;
  get_unique_chip_id_3(mac_addr + MAC_NIC_SPECIFIC_OFFSET);

  return MacAddress(mac_addr);
}

size_t MacAddress::printTo(Print & p) const
{
  char msg[32] = {0};

  uint8_t const * const ptr_mac = (this->data());

  snprintf(msg,
           sizeof(msg),
           "MAC\t%02X:%02X:%02X:%02X:%02X:%02X",
           ptr_mac[0],
           ptr_mac[1],
           ptr_mac[2],
           ptr_mac[3],
           ptr_mac[4],
           ptr_mac[5]);

  return p.write(msg);
}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/
#include "Arduino.h"

#if defined(ARDUINO_ARCH_RP2040)
#include <pico/unique_id.h>
#endif

void get_unique_chip_id_3(uint8_t * uid)
{
#if defined(ARDUINO_ARCH_SAMD)
  {
    uint32_t const samd_uid = *(volatile uint32_t*)(0x0080A048);
    memcpy(uid, &samd_uid, 3);
  }
#elif defined(ARDUINO_MINIMA) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_PORTENTA_C33)
  {
    const bsp_unique_id_t* t = R_BSP_UniqueIdGet();
    memcpy(uid, t->unique_id_bytes, 3);
  }
#elif defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4)
  {
    uint32_t const stm32_uid = HAL_GetUIDw2();
    memcpy(uid, &stm32_uid, 3);
  }
  {
    auto stm32_uid = HAL_GetUIDw2();
    memcpy(uid, &stm32_uid, 3);
  }
#elif defined(ARDUINO_ARCH_RP2040)
  {
    /* RP2040 exposes a 64-bit unique board ID derived from the flash chip's
     * unique serial number.  Use the first 3 bytes as the NIC-specific part
     * of the MAC address. */
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);
    memcpy(uid, board_id.id, 3);
  }
#else
# warning "Retrieving a unique chip ID for MAC generation is not supported on this platform."
#endif
}

#if defined(ARDUINO_PORTENTA_C33) 
extern "C" int LWIP_RAND() {
  return rand();
}
#endif
