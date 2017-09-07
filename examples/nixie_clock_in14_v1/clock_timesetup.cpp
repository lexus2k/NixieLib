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
#include "clock_timesetup.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"


static uint8_t s_hours;
static uint8_t s_minutes;
static uint8_t s_seconds;
static uint8_t g_stateMode;

void timeSetupEnterFunction()
{
    g_stateMode = 0;
    s_hours   = g_rtc.m_hours;
    s_minutes = g_rtc.m_minutes;
    s_seconds = g_rtc.m_seconds;
    NixieOs::startTimer(0,SETUP_INACTIVITY_TIMEOUT_MS);
}


void timeSetupStateFunction()
{
    g_display[0] = s_hours >> 4;   g_display[1] = s_hours & 0x0F;
    g_display[2] = s_minutes >> 4; g_display[3] = s_minutes & 0x0F;
    g_display[4] = s_seconds >> 4; g_display[5] = s_seconds & 0x0F;
    g_display.clearFlags();
    for(uint8_t i=0; i<6; i++)
    {
        if ((g_stateMode*2 == i) || (g_stateMode*2 + 1 == i))
        {
            g_display[i].blink();
        }
        else
        {
            g_display[i].noBlink();
        }
    }
}

static void timeSetupAccept()
{
    g_rtc.m_hours   = s_hours;
    g_rtc.m_minutes = s_minutes;
    g_rtc.m_seconds = s_seconds;
    g_rtc.setTime();
    acceptSound();
}

static void timeSetupReject()
{
    rejectSound();
}


void timeSetupEventFunction(SNixieEvent &event)
{
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {
        timeSetupReject();
        g_display.on();
        NixieOs::popTask();
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_5) )
    {
        timeSetupReject();
        g_display.scrollForward();
        NixieOs::popTask();
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_1) )
    {
        timeSetupAccept();
        g_display.on();
        NixieOs::popTask();
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1 ))
    {
        if (++g_stateMode > 2) g_stateMode = 0;
        NixieOs::resetTimer(0);
    }
    int8_t s = 0;
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_2) )
    {
        NixieOs::resetTimer(0);
        s = -1;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_3) )
    {
        NixieOs::resetTimer(0);
        s = 1;
    }
    switch (g_stateMode)
    {
    case 0:
        s_hours = g_rtc.toInternal(g_rtc.toDecimal(s_hours) + s);
        if (s_hours >= 0xF0)     s_hours = 0x23;
        if (s_hours >= 0x24)     s_hours = 0x00;
        break;
    case 1:
        s_minutes = g_rtc.toInternal(g_rtc.toDecimal(s_minutes) + s);
        if (s_minutes >= 0xF0)     s_minutes = 0x59;
        if (s_minutes >= 0x60)     s_minutes = 0x0;
        break;
    case 2:
        s_seconds = g_rtc.toInternal(g_rtc.toDecimal(s_seconds) + s);
        if (s_seconds >= 0xF0)     s_seconds = 0x59;
        if (s_seconds >= 0x60)     s_seconds = 0x0;
        break;
    default:
        break;
    }
}
