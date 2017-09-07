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
#include "nixie_clock.h"
#include "clock_nightmode.h"
#include "clock_diorama.h"
#include "clock_generic.h"
#include "clock_events.h"
#include "clock_buttons.h"
#include "clock_alarm_active.h"
#include "clock_lightsensor.h"
#include "clock_colorsetup.h"

void genericStateFunction()
{
    updateDioramaLights();
    NixieClock::dimLight();
    nightModeState();
    if (checkAlarm())
    {
        wakeUpClock();
        NixieOs::pushTask(CLOCK_STATE_ALARM_ACTIVE);
    }
}


bool genericEventFunction(SNixieEvent &event)
{
    if (handleDioramaEvents(event))
    {
        return true;
    }
    if (event.event == CLOCK_EVENT_BRIGHTNESS_CHANGED)
    {
        return true;
    }
    if (event.event == CLOCK_EVENT_WAKE_UP)
    {
        wakeUpClock();
        return true;
    }
    if ((event.event >= CLOCK_EVENT_BUTTON_SHORT) && g_audioClient.isPlaying())
    {
        stopMusic();
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_3))
    {
        g_display.swapSmooth();
        NixieOs::pushTask(CLOCK_STATE_BRIGHTNESS_SETUP);
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_4))
    {
        g_savedSettings.highlightOn = !g_savedSettings.highlightOn;
        updateHighlight( true, g_sensor.brightness() );
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_5))
    { 
        NixieOs::pushTask(CLOCK_DEBUG);
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_2))
    {
        g_display.scrollForward();
        g_display.blink();
        NixieOs::pushTask(CLOCK_STATE_ALARM_SETUP);
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_3))
    {  
        changeNightMode();
        saveToEeprom();
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_4))
    {
        g_display.scrollForward();
        NixieOs::pushTask(CLOCK_STATE_COLOR_SETUP);
        return true;
    }
    if ((event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_5))
    {
        g_display.scrollForward();
        NixieOs::pushTask(CLOCK_STATE_SOUND_SETUP);
        return true;
    }
    return false;
}


