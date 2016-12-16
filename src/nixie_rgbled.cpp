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

#include "nixie_rgbled.h"


////////////////////////////////////////////////////////////////////////////////////////

void NixiePwmRgbLed::off()
{
    rgb(0,0,0);
}

////////////////////////////////////////////////////////////////////////////////////////

void NixiePwmRgbLed::rgb(byte r, byte g, byte b)
{
    m_color.r = r; m_color.g = b; m_color.b = b;
    analogWrite(m_pins[0], ((uint16_t)r * m_brightness) >> (NIXIE_BRIGHTNESS_BITS + 1));
    analogWrite(m_pins[1], ((uint16_t)g * m_brightness) >> (NIXIE_BRIGHTNESS_BITS + 1));
    analogWrite(m_pins[2], ((uint16_t)b * m_brightness) >> (NIXIE_BRIGHTNESS_BITS + 1));
}

////////////////////////////////////////////////////////////////////////////////////////

void NixiePwmRgbLed::brightness(byte br)
{
    m_brightness = (br << 1);
    rgb(m_color.r, m_color.g, m_color.b);
}

////////////////////////////////////////////////////////////////////////////////////////


