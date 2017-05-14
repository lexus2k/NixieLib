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

#ifndef _NIXIE_I2C_CLIENT_H_
#define _NIXIE_I2C_CLIENT_H_

#include "nixie_types.h"

enum
{
    I2C_READ_ONLY,
    I2C_READ_WRITE,
};

class SimpleClientI2c
{
public:
    static void begin(uint8_t clientId);
    static void onRequest();
    static void onReceive();
    static void setRegisters(uint8_t *registers, uint8_t count);
    static void setRegisters(uint8_t *registers, uint8_t *accessMap, uint8_t count);
    static uint8_t receive();
    static void write(uint8_t data);
    static void update();
private:
    static uint8_t * m_regPtr;
    static uint8_t * m_regAccessPtr;
    static uint8_t   m_regCount;
    static uint8_t   m_regIndex;
    static void receiveEvent(uint8_t howMany);
    static void requestEvent();
};

#endif
