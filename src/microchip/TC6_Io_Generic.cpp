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

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "TC6_Io_Generic.h"

#include "lib/libtc6/inc/tc6.h"

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static const uint8_t FALLBACK_MAC[] = {0x00u, 0x80u, 0xC2u, 0x00u, 0x01u, 0xCCu};

#if defined(ARDUINO_SAMD_NANO_33_IOT)
static int const IRQ_PIN   =  2;
static int const RESET_PIN =  9;
static int const CS_PIN    = 10;
#else
# error "No pins defined for your board"
#endif

static SPISettings const LAN865x_SPI_SETTING{8*1000*1000UL, MSBFIRST, SPI_MODE0};

/**************************************************************************************
 * VARIABLES
 **************************************************************************************/

static size_t constexpr MAC_SIZE = 6;
static uint8_t mac[MAC_SIZE] = {0};

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

TC6_Io_Generic::TC6_Io_Generic(HardwareSPI & spi, HardwareI2C & wire)
: _spi{spi}
, _wire{wire}
, _int_in{0}
, _int_out{0}
, _int_reported{0}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

bool TC6_Io_Generic::init(uint8_t pMac[6])
{
  digitalWrite(CS_PIN, HIGH);
  pinMode(CS_PIN, OUTPUT);

  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(100);

  _spi.begin();

  _wire.begin();

  if (get_mac_address(mac)) {
    memcpy(pMac, mac, MAC_SIZE);
  } else {
    memcpy(pMac, FALLBACK_MAC, MAC_SIZE);
  }

  return true;
}

bool TC6_Io_Generic::is_interrupt_active()
{
  _int_reported = _int_in;
  return (_int_reported != _int_out);
}

void TC6_Io_Generic::release_interrupt()
{
  if (digitalRead(IRQ_PIN) == HIGH)
    _int_out = _int_reported;
}

bool TC6_Io_Generic::spi_transaction(uint8_t const * pTx, uint8_t * pRx, uint16_t const len)
{
  digitalWrite(CS_PIN, LOW);
  _spi.beginTransaction(LAN865x_SPI_SETTING);

  for (size_t b = 0; b < len; b++)
    pRx[b] = _spi.transfer(pTx[b]);

  _spi.endTransaction();
  digitalWrite(CS_PIN, HIGH);

  TC6_SpiBufferDone(0 /* tc6instance */, true /* success */);
#if 0
  Serial.print("TX = ");
    for (size_t b = 0; b < len; b++)
    {
      char msg[8] = {0};
      snprintf(msg, sizeof(msg), "%02X ", pTx[b]);
      Serial.print(msg);
    }
    Serial.println();

    Serial.print("RX = ");
    for (size_t b = 0; b < len; b++)
    {
      char msg[8] = {0};
      snprintf(msg, sizeof(msg), "%02X ", pRx[b]);
      Serial.print(msg);
    }
    Serial.println();
#endif

  return true;
}

bool TC6_Io_Generic::get_mac_address(uint8_t * p_mac)
{
  uint8_t MAC_EEPROM_I2C_SLAVE_ADDR = 0x58;
  uint8_t MAC_EEPROM_EUI_REG_ADDR = 0x9A;
  bool success = false;

  _wire.beginTransmission(MAC_EEPROM_I2C_SLAVE_ADDR);
  _wire.write(MAC_EEPROM_EUI_REG_ADDR);
  _wire.endTransmission();

  _wire.requestFrom(MAC_EEPROM_I2C_SLAVE_ADDR, MAC_SIZE);

  uint32_t const start = get_tick();

  size_t bytes_read = 0;
  while (bytes_read < MAC_SIZE && ((get_tick() - start) < 1000)) {
    if (_wire.available()) {
      p_mac[bytes_read] = _wire.read();
      bytes_read++;
    }
  }

  success = (bytes_read == MAC_SIZE);
  return success;
}

void TC6_Io_Generic::onInterrupt()
{
  _int_in++;
}