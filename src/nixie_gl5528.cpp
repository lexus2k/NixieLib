/*
    Copyright (C) 2016-2017 Alexey Dynda

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

#include "nixie_gl5528.h"
#include "nixieos.h"

bool NixieLdrGl5528::update(uint16_t ts)
{
    if (m_ts - ts < LIGHT_SENSOR_UPDATE_INTERVAL) return false;
    int val = nixieAnalogRead(m_pin);
    if (val == ADC_IN_PROGRESS) return false;
    m_ts += LIGHT_SENSOR_UPDATE_INTERVAL;
    uint8_t level;
    if (val > m_dayLevel)
        level = NIXIE_MAX_BRIGHTNESS;
    else if (val > m_roomLevel)
        level = NIXIE_MAX_BRIGHTNESS / 2 +
                 NIXIE_MAX_BRIGHTNESS / 2 * (val - m_roomLevel)/(m_dayLevel - m_roomLevel);
    else if (val > m_nightLevel)
        level = NIXIE_MAX_BRIGHTNESS  / 2 * (val - m_nightLevel)/(m_roomLevel - m_nightLevel);
    else if (val > m_nightLevel/2)
        level = 0;
    else
        level = NIXIE_MAX_BRIGHTNESS; // some invalid sensor, take maximum brightness
    m_targetBrightness = level;
    if ( m_targetBrightness > m_brightness )
    {
        m_brightness++;
    }
    else if ( m_targetBrightness < m_brightness )
    {
        m_brightness--;
    }
    return true;
}


int NixieLdrGl5528::absoluteValue()
{
    int val;
    do
    {
        val = nixieAnalogRead(m_pin);
    }
    while (val == ADC_IN_PROGRESS);
    return val;
}
