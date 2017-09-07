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
#include "clock_brightness.h"
#include "clock_lightsensor.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"

#include "nixieos.h"


#define DYNAMIC_BRIGHTNESS   0xFF

static uint8_t s_brightness;


bool isDynamicBrightness()
{
    return DYNAMIC_BRIGHTNESS == g_savedSettings.displayBrightness;
}


void updateTubesBrightness(uint8_t sensorBrightness)
{
    if ((DYNAMIC_BRIGHTNESS == g_savedSettings.displayBrightness) || (g_savedSettings.nightMode))
    {
        g_display.setBrightness( min( NIXIE_MAX_BRIGHTNESS, sensorBrightness + 6 ) );
    }
    else
    {
        g_display.setBrightness( g_savedSettings.displayBrightness );
    }
}

static void updateDisplayBrightness()
{
    if (s_brightness == DYNAMIC_BRIGHTNESS)
    {
        g_display.setBrightness( g_sensor.brightness() );
    }
    else
    {
        g_display.setBrightness( s_brightness );
    }
}


void brightnessSetupEnterFunction()
{
    NixieOs::startTimer(0, SETUP_INACTIVITY_TIMEOUT_MS);
    s_brightness = g_savedSettings.displayBrightness;
    updateDisplayBrightness();
}


void brightnessSetupStateFunction()
{
    if (s_brightness != DYNAMIC_BRIGHTNESS)
    {
        g_display[0].smoothOff();
        g_display[1].smoothOff();
        g_display[2].smoothOn();
        g_display[3].smoothOn();
        g_display[4].smoothOff();
        g_display[5].smoothOff();
        g_display[2] = s_brightness >> (NIXIE_BRIGHTNESS_BITS - 3);
        g_display[3] = s_brightness >> (NIXIE_BRIGHTNESS_BITS - 3);
    }
    else
    {
        g_display[0].smoothOff();
        g_display[1].smoothOff();
        g_display[2].smoothOn();
        g_display[3].smoothOn();
        g_display[4].smoothOff();
        g_display[5].smoothOff();
        g_display[2] = 0;
        g_display[3] = 0;
    }
    g_sensor.update(g_nixieMs);
}


static void acceptBrightness()
{
    g_savedSettings.displayBrightness = s_brightness;
}


void brightnessSetupEventFunction(SNixieEvent &event)
{
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {
        rejectSound();
        updateTubesBrightness(g_sensor.brightness());
        g_display.swapSmooth();
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_5) )
    {
        rejectSound();
        updateTubesBrightness(g_sensor.brightness());
        g_display.swapSmooth();
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_1) )
    {
        acceptSound();
        acceptBrightness();
        updateTubesBrightness(g_sensor.brightness());
        g_display.on();
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_3 ) )
    {
        NixieOs::resetTimer(0);
        if (s_brightness == DYNAMIC_BRIGHTNESS)
        {
            s_brightness = (1 << (NIXIE_BRIGHTNESS_BITS - 3));
        }
        else
        {
            s_brightness += (1 << (NIXIE_BRIGHTNESS_BITS - 3));
            if (s_brightness > NIXIE_MAX_BRIGHTNESS)
            {
                s_brightness = DYNAMIC_BRIGHTNESS;
            }
        }
        updateDisplayBrightness();
    }
}
