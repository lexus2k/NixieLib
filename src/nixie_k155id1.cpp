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

#include "nixie_k155id1.h"
#include "nixieos.h"

//////////////////////////////////////////////////////////////////////////////

void K155ID1::switchPin(uint8_t pin, uint8_t extendedPins)
{
    /* We should not set binary code out of bound. */
    /* Otherwise, this can break K155ID1 driver.    */
    if (pin < 10)
    {
        for (uint8_t i=0; i<4; i++)
        {
            if ((pin >> i) & 0x01)
                NixieOs::pinHigh(m_pins[i]);
            else
                NixieOs::pinLow(m_pins[i]);
        }
    }
    else
    {
        for (uint8_t i=0; i<4; i++)
        {
            NixieOs::pinHigh(m_pins[i]);
        }
    }
    for (uint8_t i=0; i< m_count - 4; i++)
    {
        if ((extendedPins >> i) & 0x01)
            NixieOs::pinHigh(m_pins[i+4]);
        else
            NixieOs::pinLow(m_pins[i+4]);
    }
}

//////////////////////////////////////////////////////////////////////////////

void K155ID1::init(void)
{
    for(uint8_t i=0; i<m_count; i++)
    {
        pinMode(m_pins[i], OUTPUT);
        digitalWrite(m_pins[i], LOW);
    }
}

void K155ID1::off()
{
    K155ID1::switchPin( 0x0F, 0x00 );
}
