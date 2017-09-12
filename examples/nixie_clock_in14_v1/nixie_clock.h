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

#ifndef _NIXIE_CLOCK_H_
#define _NIXIE_CLOCK_H_

//#define NIXIE_CLOCK_V1

#include "nixie_display.h"
#include "nixie_ds3232.h"
#include "nixie_buttons.h"
#include "nixie_audio_i2c.h"
#include "nixie_gl5528.h"
#include "nixie_lm35dz.h"
#include "nixie_rgbled.h"

typedef enum
{
    ACTION_CLICK,
    ACTION_ACCEPT,
    ACTION_REJECT,
} EActionSound;

typedef struct SNixieSettings
{
    SNixieColor highlightColor;
    uint8_t     colorProfile;
    bool        highlightOn;
    uint8_t     highlightBrightness;
    uint8_t     displayBrightness;
    uint8_t     nightHighlightBrightness;

    uint8_t     alarmMelody;
    bool        alarmEnable;
    uint8_t     alarmHours;
    uint8_t     alarmMinutes;

    uint8_t     clockMelody;
    bool        soundEnable;

    uint8_t     nightMode;

    int16_t     nightLevel;
    int16_t     dayLevel;
    int16_t     roomLevel;

    void        read(int base);
    void        write(int base);
} SNixieSettings;

/**
 * Variables below should be defined in main application
 */
extern    NixieDisplay         g_display;
extern    Ds3231               g_rtc;    
extern    NixieAnalogButtons   g_buttons;
extern    NixiePwmRgbLed       g_highlight;

/**
 * Variables below are defined in nixie clock module
 */
extern    SNixieSettings       g_savedSettings;
extern    NixieAudioI2c        g_audioClient;
extern    uint8_t              g_count;




class NixieClock
{
public:
    static void init();

    static void printTime(bool showSeconds);
    static void printDate();
    static void printAlarm(bool showEnable);

    static void dimLight();
};

void stopMusic();
void saveToEeprom();

#endif

