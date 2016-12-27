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

#ifndef _NIXIE_RGBLED_H_
#define _NIXIE_RGBLED_H_

#include "nixie_types.h"
#include "nixie_tube.h"


/**
 * NixiePwmRgbLed class is intended to control RGB leds connected to
 * PWM pins. For example, for Atmega328p PB1, PB2, PB3 pins can be used.
 * Be careful when connecting LED pin to microcontroller. Remember, that
 * most Atmel controllers provide 20mA at max (40mA for a short time only).
 * If you're not sure, if microcontroller is possible to provide power for
 * RGB led, please, connect LED via bi-polar transistor to controller pins.
 */
class NixiePwmRgbLed
{
public:
    /**
     * Constructor accepts array of 3 uint8_t integer numbers - pin numbers
     * to control. PIN array passed to the constructor must be defined 
     * globally, since the object remembers pointer to the array.
     */
    inline        NixiePwmRgbLed(const uint8_t pwmPins[]) { m_pins = pwmPins; };

    /**
     * Initializes pins. Should be called in setup().
     */
    inline void   init() { for (byte i=0; i<3; i++) pinMode(m_pins[i], OUTPUT); };

    /**
     * Turns off the led
     */
    void          off();

    /**
     * Sets the color of RGB LED.
     * @param[in] r - red value [0;255]
     * @param[in] g - green value [0;255]
     * @param[in] b - blue value [0;255]
     * Remember that color component are not balanced since Green light
     * has more brightness than red and blue on the same values.
     */
    void          rgb(byte r, byte g, byte b);

    /**
     * Sets RGB LED color.
     */
    inline void   rgb(const SNixieColor &color) { rgb(color.r, color.g, color.b); };

    /**
     * Set brightness of rgb leds in the range between [0,NIXIE_MAX_BRIGHTNESS]
     * @param br - brightness in range [0,NIXIE_MAX_BRIGHTNESS]. 64 is maximum brightness
     * for now.
     */
    void          brightness(byte br);
    
private:
    const byte   *m_pins;
    byte          m_brightness = NIXIE_MAX_BRIGHTNESS;
    SNixieColor   m_color = { 0, 0, 0 };
};

/**
 * NOT IMPLEMENTED.
 */
class NixieSoftRgbLed
{
  
};

#endif

