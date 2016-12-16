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

#include "nixie_i2c_client.h"
#include <Arduino.h>

#if defined(__AVR_ATtiny25__) || \
    defined(__AVR_ATtiny45__) || \
    defined(__AVR_ATtiny85__)

#include <TinyWireS.h>

#define I2CBUS_Send(x)     TinyWireS.send(x)
#define I2CBUS_            TinyWireS
#define I2CBUS_Receive()   TinyWireS.receive()
#define I2CBUS_UPDATE()    TinyWireS_stop_check()

#else

#include <Wire.h>
#define I2CBUS_Send(x)    Wire.write(x)
#define I2CBUS_           Wire
#define I2CBUS_Receive()  Wire.read()
#define I2CBUS_UPDATE()

#endif

uint8_t * SimpleClientI2c::m_regPtr = nullptr;
uint8_t * SimpleClientI2c::m_regAccessPtr = nullptr;
uint8_t   SimpleClientI2c::m_regCount = 0;
uint8_t   SimpleClientI2c::m_regIndex = 0;

void SimpleClientI2c::requestEvent()
{  
    if ( (m_regPtr != nullptr) && (m_regIndex < m_regCount) )
    {
        I2CBUS_Send(m_regPtr[m_regIndex]);
        m_regIndex++;
        if (m_regIndex >= m_regCount)
        {
            m_regIndex = m_regCount - 1;
        }
    }
    else
    {
    }
}


void SimpleClientI2c::receiveEvent(uint8_t howMany)
{
    if (howMany < 1)
    {
        return;
    }
    m_regIndex = I2CBUS_Receive();
    if ((m_regCount != 0) && (m_regIndex<m_regCount))
    {
        while (--howMany)
        {
            uint8_t data = I2CBUS_Receive();
            if ( (m_regAccessPtr != nullptr) && (m_regAccessPtr[m_regIndex] == I2C_READ_WRITE) )
            {
                m_regPtr[m_regIndex] = data;
            }
            if (++m_regIndex >= m_regCount)
            {
                m_regIndex = m_regCount - 1;
            }
        }
    }
    else
    {
        while (--howMany) I2CBUS_Receive();
    }
}


void SimpleClientI2c::begin(uint8_t clientId)
{
    I2CBUS_.begin(clientId);
    I2CBUS_.onReceive(receiveEvent);
    I2CBUS_.onRequest(requestEvent);
}


void SimpleClientI2c::onRequest()
{
}

void SimpleClientI2c::onReceive()
{
  
}

void SimpleClientI2c::setRegisters(uint8_t *registers, uint8_t count)
{
    m_regPtr = registers;
    m_regCount = count;
    m_regAccessPtr = nullptr;
}

void SimpleClientI2c::setRegisters(uint8_t *registers, uint8_t *accessMap, uint8_t count)
{
    m_regPtr = registers;
    m_regCount = count;
    m_regAccessPtr = accessMap;
}


uint8_t SimpleClientI2c::receive()
{
    I2CBUS_Receive();
}
  
void    SimpleClientI2c::write(uint8_t data)
{
    I2CBUS_Send(data);
}

void SimpleClientI2c::update()
{
    I2CBUS_UPDATE();
}

