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

#include "clock_states.h"
#include "clock_alarm_enable.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"

enum
{
    ALARM_SETUP_ENABLE,
    ALARM_SETUP_HOURS,
    ALARM_SETUP_MINUTES,
};

static bool s_alarmEnable;
static uint8_t s_alarmHours;
static uint8_t s_alarmMinutes;
static uint8_t s_alarmMelody;
static uint8_t s_state;
static uint8_t s_melodyCount;


void alarmEnableEnterFunction()
{
    NixieOs::startTimer(0, SETUP_INACTIVITY_TIMEOUT_MS);
    s_alarmEnable = g_savedSettings.alarmEnable;
    s_alarmHours = g_savedSettings.alarmHours;
    s_alarmMinutes = g_savedSettings.alarmMinutes;
    s_alarmMelody = g_savedSettings.alarmMelody;
    s_state = ALARM_SETUP_ENABLE;
    s_melodyCount = g_audioClient.getCount();
}


void alarmEnableStateFunction()
{
    g_display.clearFlags();
    g_display[0] = s_alarmHours >> 4;
    g_display[1] = s_alarmHours & 0x0F;
    g_display[1].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[2] = s_alarmMinutes >> 4;
    g_display[3] = s_alarmMinutes & 0x0F;

    if (s_state == ALARM_SETUP_ENABLE)
    {
        g_display[0].blink();
        g_display[1].blink();
        g_display[2].blink();
        g_display[3].blink();
        if (s_alarmEnable)
        {
            /* set not valid value for the tube to turn off digits */
            g_display[4].off();
            g_display[5].on();
            g_display[5] = s_alarmMelody;
            g_display[5].setFlags(TUBE_FLAG_RIGHT_DOT);
        }
        else
        {
            g_display[4].off();
            g_display[5].off();
        }
    }
    if (s_state == ALARM_SETUP_HOURS)
    {
        g_display[0].blink();
        g_display[1].blink();
        g_display[2].noBlink();
        g_display[3].noBlink();
    }
    if (s_state == ALARM_SETUP_MINUTES)
    {
        g_display[0].noBlink();
        g_display[1].noBlink();
        g_display[2].blink();
        g_display[3].blink();
    }
}


static void acceptAlarmEnable()
{
    g_savedSettings.alarmEnable = s_alarmEnable;
    g_savedSettings.alarmHours = s_alarmHours;
    g_savedSettings.alarmMinutes = s_alarmMinutes;
    g_savedSettings.alarmMelody = s_alarmMelody;
}


void alarmEnableEventFunction(SNixieEvent &event)
{
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {
        stopMusic();
        rejectSound();
        g_display.moveLeft();
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_5) )
    {
        stopMusic();
        rejectSound();
        g_display.moveLeft();
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_1) )
    {
        stopMusic();
        acceptSound();
        acceptAlarmEnable();
        g_display.swapSmooth();
        NixieOs::popTask();
        return;
    }
    if ( event.event == CLOCK_EVENT_BUTTON_SHORT )
    {
        NixieOs::resetTimer(0);
    }
    if ( s_state == ALARM_SETUP_ENABLE )
    {
        if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1 ) )
        {
            s_state = ALARM_SETUP_HOURS;
        }
        if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_2 ) )
        {
            s_alarmEnable = !s_alarmEnable;
            if (s_alarmEnable)
            {
                g_audioClient.play(s_alarmMelody - 1, 2);
            }
            else
            {
                g_audioClient.stop();
            }
        }
        if ( s_alarmEnable && (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_3 ) )
        {
            if (++ s_alarmMelody > s_melodyCount)
            {
                s_alarmMelody = 0;
            }
            if (s_alarmMelody > 0)
                g_audioClient.play( s_alarmMelody - 1 );
            else
                g_audioClient.stop();
        }
    }
    else
    {
        if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1 ) )
        {
            if (s_state == ALARM_SETUP_HOURS)
            {
                s_state = ALARM_SETUP_MINUTES;
            }
            else
            {
                s_state = ALARM_SETUP_ENABLE;
            }
        }
        int8_t s = 0;
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_2 ))
        {
            s = -1;
        }
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_3 ))
        {
            s = 1;
        }
        if (s != 0)
        {
            if (s_state == ALARM_SETUP_HOURS)
            {
                s_alarmHours = Ds3231::toInternal( Ds3231::toDecimal( s_alarmHours ) + s );
                if (s_alarmHours > 0xF0) s_alarmHours = 0x23; 
                if (s_alarmHours > 0x23) s_alarmHours = 0;
            }
            else
            {
                s_alarmMinutes = Ds3231::toInternal( Ds3231::toDecimal( s_alarmMinutes ) + s );
                if (s_alarmMinutes > 0xf0) s_alarmMinutes = 0x59;
                if (s_alarmMinutes > 0x59) s_alarmMinutes = 0;
            }
        }
    }
}


