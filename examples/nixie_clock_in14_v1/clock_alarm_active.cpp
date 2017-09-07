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
#include "clock_alarm_active.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"
#include "clock_nightmode.h"

static uint8_t g_prevSeconds;
static uint8_t g_prevMinutes;

bool checkAlarm()
{
    if (g_rtc.m_seconds != 0)
    {
        return false;
    }
    if (g_savedSettings.alarmEnable)
    {
        if ((g_rtc.m_hours == g_savedSettings.alarmHours) && 
            (g_rtc.m_minutes == g_savedSettings.alarmMinutes) &&
            (g_rtc.m_seconds == 0))
        {
            return true;
        }
    }
    if ((g_prevMinutes != g_rtc.m_minutes) && (g_rtc.m_minutes == 0))
    {
        if ((g_savedSettings.clockMelody > 0) && !isNightSilence())
            g_audioClient.sound(g_savedSettings.clockMelody - 1);
    }
    g_prevSeconds = g_rtc.m_seconds;
    g_prevMinutes = g_rtc.m_minutes;
    return false;
}

void alarmActiveEnterFunction()
{
    NixieOs::startTimer(0, ALARM_RING_DURATION_MS);
    NixieClock::printAlarm(false);
    g_display.blink();
    g_audioClient.play(g_savedSettings.alarmMelody - 1);
}


void alarmActiveStateFunction()
{
}


void alarmActiveEventFunction(SNixieEvent &event)
{
    if ( (event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0) ||
         (event.event == CLOCK_EVENT_BUTTON_SHORT) )
    {
        stopMusic();
        g_display.swapSmooth();
        NixieOs::popTask();
        return;
    }
}


