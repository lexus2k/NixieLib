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

#include "clock_nightmode.h"
#include "clock_diorama.h"
#include "nixie_clock.h"
#include "clock_brightness.h"
#include "clock_colorsetup.h"
#include "clock_lightsensor.h"
#include "clock_fine_tuning.h"
#include "nixieos.h"

enum
{
    NIGHT_MODE_NORMAL,
    NIGHT_MODE_SILENT,
    NIGHT_MODE_SLEEPING,   
};

static uint8_t      s_nightModeState = NIGHT_MODE_NORMAL;
static uint32_t     s_nightModeTs;


static bool isAlarmSoon()
{
    int16_t mins1 = Ds3231::toMinutes(g_savedSettings.alarmHours, g_savedSettings.alarmMinutes, false);
    int16_t mins2 = Ds3231::toMinutes(g_rtc.m_hours, g_rtc.m_minutes, false);
    /* Somebody is going to get up soon ? */
    if ( Ds3231::timeDelta(mins1, mins2) < ALARM_WAKE_UP_CLOCK_BEFORE_M )
    {
        return true;
    }
    return false;
}


inline bool isNobodyAtHome()
{
    /* Nobody at home ? */
    /* On Sunday and Saturday home is full of guests */
    if (g_rtc.isWeekEnd( ))
    {
        return false;
    }
    /* On work days everybody is at work */
    if ((g_rtc.m_hours>=0x11) && (g_rtc.m_hours<0x17))
    {
        return true;
    }
    return false;
}

static bool isSleepTime()
{
    /* 11pm - 6am clock wants to sleep */
    if ((g_rtc.m_hours>=0x23) || (g_rtc.m_hours<0x06))
    {
        return true;
    }
    return false;
}

/**
 * Night mode:
 *     1. The clock enables light sensor in this mode, i.e. the brightness will depend on room lightning.
 *     2. In low light conditions the clock disables sounds (except Alarm)
 *     3. In low light conditions the clock turns off the screen, if no buttons activity. 
 *     4. The clock turns off the screen from 11pm till 6am, if no buttons activity.
 *     5. The screen is automatically turned on in high light conditions.  
 */
void nightModeState()
{
    if ( g_savedSettings.nightMode == 0 )
    {
        s_nightModeState = NIGHT_MODE_NORMAL;
        return;
    }
    switch ( s_nightModeState )
    {
    case NIGHT_MODE_NORMAL:
        if ( (isSleepTime()) || (g_sensor.brightness() < LS_SILENT_CONDITIONS) )
        {
            s_nightModeState = NIGHT_MODE_SILENT;
            s_nightModeTs = g_nixieMsEx;
            break;
        }
        if (isNobodyAtHome())
        {
            if (g_nixieMsEx - s_nightModeTs > GO_TO_SILENT_TIMEOUT_MS)
            {
                enableDioramaLights(false);
                s_nightModeState = NIGHT_MODE_SLEEPING;
                g_display.scrollOff();
                s_nightModeTs = g_nixieMsEx;
            }
            break;
        }
        s_nightModeTs = g_nixieMsEx;
        break;
    case NIGHT_MODE_SILENT:
        if ( (g_sensor.brightness() > LS_SILENT_CONDITIONS) && (!isSleepTime()))
        {
            s_nightModeState = NIGHT_MODE_NORMAL;
        }
        if ( (isNobodyAtHome() || (g_sensor.brightness() < LS_SLEEP_CONDITIONS)) && !isAlarmSoon() )
        {
            /* On 1 minute timeout */
            if (g_nixieMsEx - s_nightModeTs > GO_TO_SLEEP_TIMEOUT_MS)
            {
                enableDioramaLights(false);
                s_nightModeState = NIGHT_MODE_SLEEPING;
                g_display.scrollOff();
                s_nightModeTs = g_nixieMsEx;
            }
        }
        else
        {
            s_nightModeTs = g_nixieMsEx;
        }
        break;
    case NIGHT_MODE_SLEEPING:
        /* On 1 minute timeout */
        if ((uint32_t)(g_nixieMsEx - s_nightModeTs) > 2000)
        {
            g_display.powerOff();
            s_nightModeTs = g_nixieMsEx;
        }
        if ( (g_sensor.brightness() > LS_SLEEP_CONDITIONS) && !isNobodyAtHome())
        {
            wakeUpClock();
        }
        if (isAlarmSoon())
        {
            wakeUpClock();
        }
        break;
    default:
        break;
    }
}

void wakeUpClock()
{
    if (s_nightModeState == NIGHT_MODE_SLEEPING)
    {
        enableDioramaLights(true);
        g_display.off();
        g_display.powerOn();
        g_display.scrollOn();
        s_nightModeState = NIGHT_MODE_SILENT;
        s_nightModeTs = g_nixieMsEx;
    }
}

void changeNightMode()
{
    g_display.powerOff();
    if ( g_savedSettings.nightMode == 0 )
    {
        g_audioClient.action(ACTION_ACCEPT);
        g_savedSettings.nightMode = 1;
        s_nightModeTs = g_nixieMsEx;
        g_display.swapSmooth();
        updateHighlight( false, g_sensor.brightness() );
    }
    else
    {
        g_savedSettings.nightMode = 0;
        g_audioClient.action(ACTION_REJECT);
        wakeUpClock();
        g_display.swapSmooth();
        updateTubesBrightness( g_sensor.brightness() );
        updateHighlight( true, g_sensor.brightness() );
    }
    g_display.powerOn();
}

bool isNightSilence()
{
    return s_nightModeState != NIGHT_MODE_NORMAL;
}

bool isNightSleeping()
{
    return s_nightModeState == NIGHT_MODE_SLEEPING;
}


void kickNightMode()
{
    s_nightModeTs = g_nixieMsEx;
}
