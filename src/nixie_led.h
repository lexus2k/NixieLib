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

#ifndef _NIXIE_LED_H_
#define _NIXIE_LED_H_

#include "nixie_types.h"

class NixieLed
{
public:
    /**
     * @brief Creates NixieLed object.
     * @param pin - pin number to control led.
     */
    inline        NixieLed(uint8_t pin) { m_pin = pin; };

    /**
     * @brief Switches LED off
     */
    void inline   off()              { digitalWrite(m_pin, LOW); };

    /**
     * @brief Switches LED on
     */
    void inline   on()               { digitalWrite(m_pin, HIGH); };
private:
    uint8_t          m_pin;    
};

#endif

