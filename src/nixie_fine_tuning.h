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

#ifndef _NIXIE_FINE_TUNING_H_
#define _NIXIE_FINE_TUNING_H_

/** How much bits to use for display brightness */
#define NIXIE_BRIGHTNESS_BITS   5

/** How quickly each digit switches to another when numerating digits in milliseconds */
#define SCROLL_INTERVAL         38

/** The blinking internal of the tube in milliseconds. Must be power of 2. */
#define BLINK_INTERVAL          1024
#define BLINK_DIM_INTERVAL      8

/**
 *  Maximum brightness, supported by the NixieTube class.
 *  At present, it is range [0..32]
 */
#define NIXIE_MAX_BRIGHTNESS    (1<<NIXIE_BRIGHTNESS_BITS)

/** Interval in milliseconds for updating Nixie Tube brightness */
#define DIMMING_INTERVAL        (20)

/*
 * IN-12/IN-14 Datasheet.
 * Impulse mode: 70us / refresh frequency 1-1.8kHz
 */

/* Burning time for each digit.
 * When 1 digit is ON, other are OFF. This means that during full cycle 
 * bulb is OFF for CHANGE_INTERVAL*5.
 * IMPULSE_WIDTH should be greater than 70us (according to IN-14 spec) and working
 * frequency should be below 1-1.8kHz, otherwise the gas in the the tube will be visible.
 * On other way, Atmega328 supports only 4us accuracy.
 * Nixie Display module uses 32 levels of brightness, thus to have 25% accuracy on low brightness
 * minimum change interval should be 100/25 * 4 * 64 = 512.
 */
//#define CHANGE_INTERVAL   3000 // ~ 55Hz for each tube
//#define CHANGE_INTERVAL   2000 // ~ 83Hz for each tube
#define CHANGE_INTERVAL   1600 // ~ 104Hz for each tube
//#define CHANGE_INTERVAL   1200 // ~ 138Hz for each tube
//#define CHANGE_INTERVAL   800 // ~ 160Hz for each tube

#define MIN_DESIRED_BRIGHTNESS  ( 3 * NIXIE_MAX_BRIGHTNESS / 32 )
#define MIN_NIXIE_IMPULSE_SPEC  ( 70 )  // 70us per spec
#if CHANGE_INTERVAL * MIN_DESIRED_BRIGHTNESS / NIXIE_MAX_BRIGHTNESS < MIN_NIXIE_IMPULSE_SPEC
   #error "Minimum nixie impulse should be at least 70us. Please increase change interval"
#endif


#endif


