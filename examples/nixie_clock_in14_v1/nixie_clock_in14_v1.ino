/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of Nixie Clock Library.

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

/**
 * Nixie clock v1.1 
 * 1. Indicators:    - IN-14 (6 Digits, 4 buttons), uses right comma as digits separator
 * 2. Atmega328P     - heart of the system
 * 3. Attiny85       - sound processor, connected over I2C, with 2 piezo-buzzers
 * 4. DS3232N        - precise real time clock, connected over I2C
 * 5. LM35DZ         - temperature sensor with low self-heating
 * 6. GL5516         - photo-resistor (light sensor)
 * 7. HH-500CRGBC503 - RGB Light diode
 * 8. 4 Buttons
 * 
 * .---------------------------------------------------------------------------------------------------------------.
 * |                                          Buttons description                                                  |
 * |------------.-----------------------------------------------.--------------------------------------------------|
 * |  Button    |                   Main screen                 |                   Settings screen                |
 * |------------|------ Short Press -----.----- Long Hold ------|----- Short Press -------.----- Long Hold --------|
 * |  BUTTON 1  |  Clock mode selection  |  Time/Date setup     |  Select mode            |  Save settings         |
 * |  BUTTON 2  |  Alarm on / off        |  Alarm clock setup   |  Minus / R              |  Apply, but don't save |
 * |  BUTTON 3  |  Brightness mode       |  Clock melody setup  |  Plus  / G              |                        |
 * |  BUTTON 4  |  Highlight on / off    |  Hightlight setup    |          B              |  Cancel settings       |
 * '------------'------------------------|----------------------'-------------------------'------------------------|
 * |         Clock modes                 |                               Comments                                  |
 * |-------------------------------------|-------------------------------------------------------------------------|
 * | Time only                           |  Displays only time. All functions are working                          |
 * | Date only                           |  Displays date for 10 seconds, then goes to Time only mode              |
 * | Temperature                         |  Displays temperature for 10 seconds, then goes to Time only mode       |
 * | Time And Date                       |  Displays date for 10 seconds, then time for 15 seconds                 |
 * | Night mode *                        |  Special mode for low light condition and rest                          |
 * '-------------------------------------'-------------------------------------------------------------------------'
 * Night mode:
 *     1. The clock enables light sensor in this mode, i.e. the brightness will depend on room lightning.
 *     2. In low light conditions the clock disables sounds (except Alarm)
 *     3. In low light conditions the clock turns off the screen, if no buttons activity. 
 *     4. The clock turns off the screen from 11pm till 6am, if no buttons activity.
 *     5. The screen is automatically turned on in high light conditions.

// ATMEL ATMEGA8 & 168 & 328 / ARDUINO
//
//                  +-\/-+
//    10kOm   PC6  1|    |28  PC5 (AI 5) SCL
// MinL (D 0) PD0  2|    |27  PC4 (AI 4) SDA
// SecH (D 1) PD1  3|    |26  PC3 (AI 3) Light sens
// SecL (D 2) PD2  4|    |25  PC2 (AI 2) Temp sens
// MinH (D 3) PD3  5|    |24  PC1 (AI 1) Buttons
// HourH(D 4) PD4  6|    |23  PC0 (AI 0) [FREE]
//            VCC  7|    |22  GND
//            GND  8|    |21  AREF
//            PB6  9|    |20  AVCC
//            PB7 10|    |19  PB5 (D 13) Bit2
// HourL(D 5) PD5 11|    |18  PB4 (D 12) Bit1
// rDot (D 6) PD6 12|    |17  PB3 (D 11) LedG
// Bit0 (D 7) PD7 13|    |16  PB2 (D 10) LedB
// Bit3 (D 8) PB0 14|    |15  PB1 (D 9)  LedR
//                  +----+

 
 */

#include <Wire.h>
#include "nixie_gl5528.h"
#include "nixie_clock.h"
#include "nixie_k155id1.h"
#include "clock_states.h"
#include "clock_generic.h"
#include "clock_buttons.h"
#include "clock_bluetooth.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_temponly.h"
#include "clock_timeonly.h"
#include "clock_dateonly.h"
#include "clock_timedate.h"
#include "clock_timesetup.h"
#include "clock_datesetup.h"
#include "clock_colorsetup.h"
#include "clock_alarm_enable.h"
#include "clock_alarm_active.h"
#include "clock_sound_setup.h"
#include "clock_brightness.h"
#include "clock_nightmode.h"
#include "clock_lightsensor.h"
#include "nixieos.h"


#ifdef NIXIE_CLOCK_V1
const byte    tubePins[]    =  { 4,   5,   3,   0,   1,   2 };
const byte    driverPins[]  =  { 7, 12,  13,  8, 6 };
// TODO: should be returned back to old value
const byte    in14driverMap[11]  = { 3, 2, 7, 4, 1, 9, 8, 0, 5, 6, 15 };
const byte    ledPins[]                  =  { 9,  10,  11                }; // R, G, B
#else
const byte    tubePins[]    =  { 4,   5,   3,   1,   0,   2 };
const byte    driverPins[]  =  { 13, 8,  7,  12, 6 };
                                //   0  1  2  3  4  5  6  7  8  9  OFF //
const byte    in14driverMap[11]  = { 3, 2, 7, 4, 1, 9, 8, 0, 5, 6, 15 };
const byte    ledPins[]                  =  { 10,  9,  11  }; // R, G, B
#endif

NixiePwmRgbLed      g_highlight(ledPins);
Ds3231              g_rtc;
K155ID1             g_driver(driverPins, 1); // and 1 extendedPin to control dot
NixieDisplay        g_display(g_driver, tubePins, in14driverMap, g_count);
     
const SNixieStateInfo PROGMEM clockStates[] =
{
    NIXIEOS_TASK_(CLOCK_STATE_TIME_ONLY, timeOnly),
    NIXIEOS_TASK (CLOCK_STATE_DATE_ONLY, dateOnly),
    NIXIEOS_TASK (CLOCK_STATE_TEMP_ONLY, temperatureOnly),
    NIXIEOS_TASK (CLOCK_STATE_TIME_AND_DATE, timeDate),
    NIXIEOS_TASK (CLOCK_STATE_TIME_SETUP, timeSetup),
    NIXIEOS_TASK (CLOCK_STATE_DATE_SETUP, dateSetup),
    NIXIEOS_TASK (CLOCK_STATE_COLOR_SETUP, colorSetup),
    NIXIEOS_TASK (CLOCK_STATE_BRIGHTNESS_SETUP, brightnessSetup),
    NIXIEOS_TASK (CLOCK_STATE_ALARM_SETUP, alarmEnable),
    NIXIEOS_TASK (CLOCK_STATE_SOUND_SETUP, soundSetup),
    NIXIEOS_TASK (CLOCK_STATE_ALARM_ACTIVE, alarmActive),
    NIXIEOS_TASK_END
};


///////////////////////////////////////////////////////////////////////////////////
//          STATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

#include <EEPROM.h>

void debugStateFunction()
{
//    uint16_t val = analogRead(1);
    uint16_t val =  EEPROM.read(0x00 + 0x20);
    g_display[0] = (val / 1000) %10;
    g_display[1] = (val / 100)  %10;
    g_display[2] = (val / 10)   %10;
    g_display[3] = (val / 1)    %10;
}

void setup()
{
    analogReference( DEFAULT );
    NixieOs::setup( clockStates );
    Wire.begin();
    bluetoothInit();
//    NixieOs::run();
    NixieClock::init();
    g_rtc.init();
    g_audioClient.init();
    g_driver.init();
    g_display.init();
    g_highlight.init();
    g_display.off();
    g_display.powerOff();
    initButtons();
    g_sensor.init();
    g_sensor.setNightAdcLevel( g_savedSettings.nightLevel );
    g_sensor.setDayAdcLevel( g_savedSettings.dayLevel );
    g_sensor.setRoomAdcLevel( g_savedSettings.roomLevel );

    updateHighlight(false, g_sensor.brightness() );
    updateTubesBrightness( g_sensor.brightness() );
    /* Do not initialize HTU21 directly after power up as other I2C devices may not be ready */
    g_htu21.init();
    g_display.powerOn();
    g_display.scrollOn();
    g_rtc.refreshTime(DS3231_REFRESH_FORCED);
    /* Setup task to start with */
    NixieOs::pushTask( CLOCK_STATE_TIME_ONLY );
}


void loop()
{
    g_rtc.refreshTime(DS3231_REFRESH_REDUCED);
    NixieOs::run();
    g_buttons.update();
    g_display.render();
    bluetoothCheck();
}
