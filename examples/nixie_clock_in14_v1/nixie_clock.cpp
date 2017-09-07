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

#include "nixie_clock.h"
#include "clock_states.h"
#include "nixieos.h"
#include "clock_events.h"
#include "clock_buttons.h"
#include "clock_sounds.h"
#include "clock_brightness.h"
#include "clock_nightmode.h"
#include "clock_lightsensor.h"
#include "clock_colorsetup.h"
#include "clock_fine_tuning.h"
#include <EEPROM.h>

SNixieSettings       g_savedSettings;
SNixieSettings       g_activeSettings;
NixieAudioI2c        g_audioClient;
uint8_t              g_count = 6;




void SNixieSettings::read(int base)
{
    highlightColor.r    = EEPROM.read(base + 0x10);
    highlightColor.g    = EEPROM.read(base + 0x11);
    highlightColor.b    = EEPROM.read(base + 0x12);
    highlightOn         = EEPROM.read(base + 0x13);
    highlightBrightness = EEPROM.read(base + 0x14);
    colorProfile        = EEPROM.read(base + 0x15);
    displayBrightness   = EEPROM.read(base + 0x16);
    nightMode           = EEPROM.read(base + 0x17);
    /* No highlight on default settings */
    if (nightMode == 0xFF)
    {
        nightMode = 0;
    }
    if (highlightBrightness == 0xFF)
    {
        highlightOn = true;
        highlightBrightness = NIXIE_MAX_BRIGHTNESS;
        highlightColor.r    = 64;
        highlightColor.g    = 168;
        highlightColor.b    = 0;
    }
    if (displayBrightness == 0xFF)
    {
        displayBrightness = 0xFF;
    }
  
    alarmMelody         = EEPROM.read(base + 0x20);
    alarmEnable         = EEPROM.read(base + 0x21);
    alarmHours          = EEPROM.read(base + 0x22);
    alarmMinutes        = EEPROM.read(base + 0x23);
    if (alarmMelody == 0xFF)
    {
      alarmMelody  = 1;
      alarmEnable  = false;
      alarmHours   = 7;
      alarmMinutes = 0;
    }
  
    clockMelody         = EEPROM.read(base + 0x30);
    soundEnable         = EEPROM.read(base + 0x31);
    if (clockMelody == 0xFF)
    {
      soundEnable = true;
      clockMelody  = 0;
    }

    EEPROM.get(base + 0x40, nightLevel);
    EEPROM.get(base + 0x42, roomLevel);
    EEPROM.get(base + 0x44, dayLevel);
    if ((nightLevel < 0) || (roomLevel < 0) || (dayLevel < 0))
    {
        nightLevel = LS_DEFAULT_NIGHT_LEVEL;
        roomLevel  = LS_DEFAULT_ROOM_LEVEL;
        dayLevel   = LS_DEFAULT_DAY_LEVEL;
    }
}

void SNixieSettings::write(int base)
{
    EEPROM.update(base + 0x10, highlightColor.r);
    EEPROM.update(base + 0x11, highlightColor.g);
    EEPROM.update(base + 0x12, highlightColor.b);
    EEPROM.update(base + 0x13, highlightOn);
    EEPROM.update(base + 0x14, highlightBrightness);
    EEPROM.update(base + 0x15, colorProfile);
    EEPROM.update(base + 0x16, displayBrightness);
    EEPROM.update(base + 0x17, nightMode);
  
    EEPROM.update(base + 0x20, alarmMelody);
    EEPROM.update(base + 0x21, alarmEnable);
    EEPROM.update(base + 0x22, alarmHours);
    EEPROM.update(base + 0x23, alarmMinutes);
    
    EEPROM.update(base + 0x30, clockMelody);
    EEPROM.update(base + 0x31, soundEnable);

    EEPROM.put(base + 0x40, nightLevel);
    EEPROM.put(base + 0x42, roomLevel);
    EEPROM.put(base + 0x44, dayLevel);
}


void NixieClock::init()
{
    g_savedSettings.read(0x0);
}


void NixieClock::dimLight()
{
    static uint8_t lastBrightness = g_sensor.brightness();
    if (g_sensor.update(g_nixieMs))
    {
        if (g_sensor.brightness() != lastBrightness)
        {
            lastBrightness = g_sensor.brightness();
            NixieOs::sendEvent(CLOCK_EVENT_BRIGHTNESS_CHANGED, lastBrightness);
            updateTubesBrightness( lastBrightness );
            updateHighlight(true, lastBrightness);
        }
    }
}


void NixieClock::printTime(bool showSeconds)
{
    g_display[0] = g_rtc.m_hours >> 4;   g_display[1] = g_rtc.m_hours & 0x0F;
    g_display[2] = g_rtc.m_minutes >> 4; g_display[3] = g_rtc.m_minutes & 0x0F;
    g_display[4] = g_rtc.m_seconds >> 4; g_display[5] = g_rtc.m_seconds & 0x0F;
    if (showSeconds)
    {
        if (g_rtc.m_seconds & 0x01)
        {
            g_display[1].setFlags(TUBE_FLAG_RIGHT_DOT);
            g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
        }
        else
        {
            g_display[1].clearFlags(TUBE_FLAG_RIGHT_DOT);
            g_display[3].clearFlags(TUBE_FLAG_RIGHT_DOT);
        }
    }
    g_display[5].clearFlags(TUBE_FLAG_RIGHT_DOT);
}


void NixieClock::printDate()
{
    g_display[0] = g_rtc.m_day >> 4;      g_display[1] = g_rtc.m_day & 0xF;
    g_display[1].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[2] = g_rtc.m_month >> 4;    g_display[3] = g_rtc.m_month & 0x0F;
    g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[4] = (g_rtc.m_year >> 4);  g_display[5] = g_rtc.m_year & 0x0F;
    g_display[5].setFlags(TUBE_FLAG_RIGHT_DOT);
}


void NixieClock::printAlarm(bool showEnable)
{
    g_display.clearFlags();
    g_display[0] = g_savedSettings.alarmHours >> 4;
    g_display[1] = g_savedSettings.alarmHours & 0x0F;
    g_display[1].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[2] = g_savedSettings.alarmMinutes >> 4;
    g_display[3] = g_savedSettings.alarmMinutes & 0x0F;
    if (showEnable)
    {
        /* set not valid value for the tube to turn off digits */
        g_display[4].on();
        g_display[4] = 10;
        g_display[4].setFlags(g_savedSettings.alarmEnable ? TUBE_FLAG_RIGHT_DOT : 0);
        g_display[5].on();
        g_display[5] = 10;
        g_display[5].setFlags(g_savedSettings.alarmEnable ? TUBE_FLAG_RIGHT_DOT : 0);
    }
    else
    {
        g_display[4].off();
        g_display[5].off();
    }
}


///////////////////////////////////////////////////////////////////////////////////
//          HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

void saveToEeprom()
{
    g_savedSettings.write(0x0);
}

void loadFromEeprom()
{
    g_savedSettings.read(0x0);
}

void stopMusic()
{
    g_audioClient.stop();
}
