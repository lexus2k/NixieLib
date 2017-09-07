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
#include "clock_datesetup.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"

static uint8_t s_year;
static uint8_t s_month;
static uint8_t s_day;
static uint8_t g_stateMode;

void dateSetupEnterFunction()
{
    g_stateMode = 0;
    s_day    = g_rtc.m_day;
    s_month  = g_rtc.m_month;
    s_year   = g_rtc.m_year;
    NixieOs::startTimer(0,SETUP_INACTIVITY_TIMEOUT_MS);
}


void dateSetupStateFunction()
{
    NixieClock::dimLight();
    g_display.clearFlags();
    g_display[0] = s_day >> 4;      g_display[1] = s_day & 0xF;
    g_display[1].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[2] = s_month >> 4;    g_display[3] = s_month & 0x0F;
    g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[4] = (s_year >> 4);  g_display[5] = s_year & 0x0F;
    g_display[5].setFlags(TUBE_FLAG_RIGHT_DOT);
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


static void dateSetupAccept()
{
    g_rtc.m_day    = s_day;
    g_rtc.m_month  = s_month;
    g_rtc.m_year   = s_year;
    g_rtc.setDate();
    acceptSound();
}

static void dateSetupReject()
{
    rejectSound();
}


void dateSetupEventFunction(SNixieEvent &event)
{
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {
        dateSetupReject();
        g_display.on();
        NixieOs::popTask();
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_5) )
    {
        dateSetupReject();
        g_display.scrollForward();
        NixieOs::popTask();
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_1) )
    {
        dateSetupAccept();
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
        s_day = Ds3231::toInternal( Ds3231::toDecimal( s_day ) + s );
        if (s_day == 0x00) s_day = 0x31;
        if (s_day > 0x31) s_day = 1;
        break;
    case 1:
        s_month = Ds3231::toInternal( Ds3231::toDecimal( s_month ) + s );
        if (s_month == 0x00) s_month = 12;
        if (s_month > 0x12) s_month = 1;
        break;
    case 2:
        s_year = Ds3231::toInternal( Ds3231::toDecimal( s_year ) + s );
        if (s_year > 0xF0) s_year = 0x99;
        if (s_year > 0x99) s_year = 0x0;
        break;
    default:
        break;
    }
}
