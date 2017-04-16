/*
    Copyright (C) 2016 Alexey Dynda

    This file is part of Nixie Library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nixie_htu21.h"

#if !defined(__AVR_ATtiny25__) && \
    !defined(__AVR_ATtiny45__) && \
    !defined(__AVR_ATtiny85__)
#include <Wire.h>

void NixieHTU21::init()
{
    /* Reset sensor */
    Wire.beginTransmission(HTU21_I2CADDR);
    Wire.write(HTU21_RESET);
    if (Wire.endTransmission() != 0)
    {
        m_no_device = true;
    }
    else
    {
        /* Wait for reset to complete */
        delay(15);
        Wire.beginTransmission(HTU21_I2CADDR);
        Wire.write(HTU21_READREG);
        Wire.endTransmission();
        Wire.requestFrom(HTU21_I2CADDR, 1);
        m_no_device = (Wire.read() != 0x2);
    }
}

int16_t NixieHTU21::getTemperature()
{
    if (m_no_device)
    {
        return -32000;
    }
    Wire.beginTransmission(HTU21_I2CADDR);
    Wire.write(HTU21_READTEMP);
    Wire.endTransmission();
  
    /* Delay to wait for measurement results. *
     * 50ms is max value for 14-bit           */
    delay(50);
  
    Wire.requestFrom(HTU21_I2CADDR, 3);
    uint16_t temp = Wire.read();
    temp <<= 8;
    temp |= Wire.read();

    Wire.read(); // reading crc

    /* Formula: 175.72*temp/65536 - 46.85

    /* in hundreds C */
    return (((int32_t)temp*17572) >> 16 ) - 4685;
}


int16_t NixieHTU21::getHumidity()
{
    if (m_no_device)
    {
        return -32000;
    }
    Wire.beginTransmission(HTU21_I2CADDR);
    Wire.write(HTU21_READHUM);
    Wire.endTransmission();
  
    /* Delay to wait for measurement results. *
     * 16ms is max value for 12-bit           */
    delay(16);
  
    Wire.requestFrom(HTU21_I2CADDR, 3);
    uint16_t humidity = Wire.read();
    humidity <<= 8;
    humidity |= Wire.read();

    /* Reading crc */
    Wire.read();

    /* Formula: 125*humidity/65536 - 6

    /* in hundreds C */
    return (((int32_t)humidity*125) >> 16 ) - 6;
}

#endif
