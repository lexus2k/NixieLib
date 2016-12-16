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

#include "nixie_ds3232.h"

#if !defined(__AVR_ATtiny25__) && \
    !defined(__AVR_ATtiny45__) && \
    !defined(__AVR_ATtiny85__)

#include <Wire.h>

void Ds3231::init()
{
    /* Init DS3232/DS3231 */
    Wire.beginTransmission(I2C_ADDR_DS3231);
    Wire.write(0x0E); // Write to control register
    Wire.write(0b00011100);
    Wire.write(0b00110000);
    Wire.endTransmission();
    getDateTime();
}

Ds3231& Ds3231::operator  ++() 
{
    if (++m_seconds > 59)
    {
        m_seconds -= 60;
        if (++m_minutes > 59)
        {
            m_minutes -= 60;
            if (++m_hours > 23) m_hours -= 24;
        }
    }
    return *this;
}


void Ds3231::getDateTime()
{
    Wire.beginTransmission(I2C_ADDR_DS3231);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR_DS3231, 7);
  
    if (Wire.available())
    {
        m_seconds     = Wire.read();
        m_minutes     = Wire.read();
        m_hours       = Wire.read();
        m_day_of_week = Wire.read();
        m_day         = Wire.read();
        m_month       = Wire.read() & 0x1F;
        m_year        = Wire.read();
    }
}

void Ds3231::setDateTime()
{
    Wire.beginTransmission(I2C_ADDR_DS3231);
    Wire.write(0x00);
    Wire.write(m_seconds);
    Wire.write(m_minutes);
    Wire.write(m_hours);
    Wire.write(m_day_of_week);
    Wire.write(m_day);
    Wire.write(m_month);
    Wire.write(m_year);
    Wire.endTransmission();
}

void    Ds3231::setDate()
{
    Wire.beginTransmission(I2C_ADDR_DS3231);
    Wire.write(0x03);
    Wire.write(m_day_of_week);
    Wire.write(m_day);
    Wire.write(m_month);
    Wire.write(m_year);
    Wire.endTransmission();
}

void    Ds3231::setTime()
{
    Wire.beginTransmission(I2C_ADDR_DS3231);
    Wire.write(0x00);
    Wire.write(m_seconds);
    Wire.write(m_minutes);
    Wire.write(m_hours);
    Wire.endTransmission();
}

int16_t Ds3231::getTemp()
{
    //temp registers (11h-12h) get updated automatically every 64s
    Wire.beginTransmission(I2C_ADDR_DS3231);
    Wire.write(0x11);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR_DS3231, 2);
    if(Wire.available())
    {
        uint8_t temp = Wire.read();
        return (((temp & 0x7F) << 2) | (Wire.read() >> 6)) * ((temp & 0x80) ? -1: 1);
    }
    else
        return 0;
}

uint8_t Ds3231::toInternal(uint8_t d)
{
    if (d > 127)
    {
        return 0xFF;
    }
    return ((d / 10) << 4) | (d % 10);
}

uint8_t Ds3231::toDecimal(uint8_t i)
{
    return (i & 0x0F) + (i>>4) * 10;
};

int16_t Ds3231::toMinutes(uint8_t hours, uint8_t minutes, bool decimal)
{
    if (!decimal)
    {
         hours = toDecimal(hours);
         minutes = toDecimal(minutes);
    }
    return ((int16_t)hours)*60 + (int16_t)minutes;
}

int16_t Ds3231::timeDelta(int16_t min1, int16_t min2)
{
    int16_t delta1 = min2 - min1;
    int16_t delta2;
    if (delta1 < 0)
    {
        delta1 = - delta1;
        delta2 = 24*60 + min2 - min1;
    }
    else
    {
        delta2 = 24*60 + min1 - min2;
    }
    return delta1 < delta2 ? delta1: delta2;
}

#endif




