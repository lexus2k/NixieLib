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
#include "clock_timedate.h"
#include "clock_generic.h"
#include "clock_events.h"
#include "nixie_clock.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"


static uint8_t g_stateMode;
static uint8_t g_prevSeconds;

void timeDateEnterFunction()
{
    g_stateMode = 0;
    NixieOs::startTimer(0,TIME_SCREEN_TIMEOUT_MS);
    g_prevSeconds = g_rtc.m_seconds;
}

void timeDateStateFunction()
{
    genericStateFunction();
    if (g_stateMode == 0)
    {
        if ((g_prevSeconds & 0xF0) != (g_rtc.m_seconds & 0xF0))
        {
            g_display[4].scrollBack(TUBE_NORMAL);
            g_display[5].scrollBack(TUBE_NORMAL);
        }
        NixieClock::printTime(g_prevSeconds != g_rtc.m_seconds);
    }
    else
    {
        NixieClock::printDate();
    }
    g_prevSeconds = g_rtc.m_seconds;
}


void timeDateEventFunction(SNixieEvent &event)
{
    if ( genericEventFunction(event) )
    {
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_1) )
    {
        NixieOs::pushTask(CLOCK_STATE_TIME_SETUP);
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1) )
    {
        g_display.moveLeft();
        NixieOs::switchTask(CLOCK_STATE_TIME_ONLY);
    }
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {
        if ( g_stateMode == 0 )
        {
            g_stateMode = 1;
            NixieOs::startTimer(0,DATE_SCREEN_TIMEOUT_MS);
            g_display.randomEffect();
        }
        else
        {
            g_stateMode = 0;
            NixieOs::startTimer(0,TIME_SCREEN_TIMEOUT_MS);
            g_display.moveRight();
        }
    }
}
