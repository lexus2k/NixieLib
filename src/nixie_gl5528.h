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

#ifndef _NIXIE_LDR_GL5528_H_
#define _NIXIE_LDR_GL5528_H_

#include "nixie_booster.h"
#include "nixie_tube.h"
/** Update interval in milliseconds. Every 1 second */
#define LIGHT_SENSOR_UPDATE_INTERVAL   1000

const int LIGHT_LEVEL_MAX    = 1023; // 5V
const int LIGHT_LEVEL_ROOM   = 800; //808;  // 3.95V
const int LIGHT_LEVEL_NIGHT  = 562;  // 2.75V

/**
 * @brief NixieLdrGl5528 interacts with light sensor.
 *
 * GL 5528 is standard light sensor, used in many projects.
 * This class can be turned. However, default schematics to use is:
 *                   --------------- +VCC
 *                  |           |
 *               11 kOhm     GL5528
 *                  |           |
 *   ADC Pin -------------------|
 *                  |           |
 *               11 kOhm      100 nF
 *                  |           |
 *                   ----------------- GND
 */
class NixieLdrGl5528
{
public:
    /**
     * Pass ADC pin number to use as the only parameter
     */
    inline NixieLdrGl5528(uint8_t adcPin) { m_pin = adcPin; };

    /**
     * This method should be called in setup().
     */
    inline void init() { m_ts = millis(); };

    /**
     * Returns true if sensor data are updated.
     * @note Remember that light sensor is polled every second.
     */
    bool update(uint16_t ts);

    /**
     * Returns current brightness detected in the range [0..NIXIE_MAX_BRIGHTNESS]
     * @returns brightness in the range [0..32].
     * @see NIXIE_MAX_BRIGHTNESS
     */
    inline uint8_t brightness() { return m_brightness; };

    /**
     * Sets Adc value, which corresponds to room light.
     * @param[in] level - ADC level to set. For arduino it is usually in the range [0..1023]
     */
    void setRoomAdcLevel (int16_t level) { m_roomLevel = level; };

    /**
     * Sets Adc value, which corresponds to night (complete darkness).
     * @param[in] level - ADC level to set. For arduino it is usually in the range [0..1023]
     */
    void setNightAdcLevel(int16_t level) { m_nightLevel = level; };

    /**
     * Sets Adc value, which corresponds to day light (most brightness level, which can be registered).
     * @param[in] level - ADC level to set. For arduino it is usually in the range [0..1023]
     */
    void setDayAdcLevel  (int16_t level) { m_dayLevel = level; };
private:
    int16_t     m_nightLevel  = LIGHT_LEVEL_NIGHT;
    int16_t     m_roomLevel   = LIGHT_LEVEL_ROOM;
    int16_t     m_dayLevel    = LIGHT_LEVEL_MAX;
    uint8_t     m_pin;
    uint8_t     m_brightness       = (NIXIE_MAX_BRIGHTNESS);
    uint8_t     m_targetBrightness = (NIXIE_MAX_BRIGHTNESS);
    uint16_t    m_ts;
};

#endif

