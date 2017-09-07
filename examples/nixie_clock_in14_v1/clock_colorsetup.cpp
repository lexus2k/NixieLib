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
#include "clock_colorsetup.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_nightmode.h"
#include "clock_lightsensor.h"
#include "clock_fine_tuning.h"

static const SNixieColor colorProfiles[] =
{
    {255,255,255},  // White
    {255,  0,  0},  // Red
    {0,  255,  0},  // Green
    {0,  0,  255},  // Blue
    {255,255,  0},  // Yellow
    {255,  0,200},  // Purple
    {128,  0,100},  // Light purple
};


static uint8_t        s_colorProfile;
static SNixieColor    s_color;
static bool           s_colorOn;
static uint8_t        s_brightness;

void updateHighlight(bool useLightSensor, uint8_t sensorBrightness)
{
    /** Enable highlight only in the following cases:
     *  1. highlight is ON AND
     *     1.1. NightMode allows highlight
     *     1.2. OR useLightSensor == false (means that this is some setup mode, not normal functioning)
     */
    if (!g_savedSettings.highlightOn)
    {
        g_highlight.off();
        return;
    }
    if (isNightSleeping())
    {
        if ( sensorBrightness < 5 ) sensorBrightness = 5;
        g_highlight.brightness( sensorBrightness );
    }
    else if (useLightSensor)
    {
        if ( sensorBrightness < 5 ) sensorBrightness = 5;
        g_highlight.brightness(sensorBrightness);
    }
    else
    {
        g_highlight.brightness(g_savedSettings.highlightBrightness);
    }
    if (g_savedSettings.colorProfile == 0xFF)
    {
        g_highlight.rgb(g_savedSettings.highlightColor);
    }
    else
    {
        g_highlight.rgb(colorProfiles[g_savedSettings.colorProfile]);
    }
}


void colorSetupEnterFunction()
{
    NixieOs::startTimer(0,SETUP_INACTIVITY_TIMEOUT_MS);
    s_colorProfile = g_savedSettings.colorProfile;
    s_color        = g_savedSettings.highlightColor;
    s_colorOn      = g_savedSettings.highlightOn;
    s_brightness   = g_savedSettings.highlightBrightness;
}


void colorSetupStateFunction()
{
    if (!s_colorOn)
    {
        g_display[0].off();
        g_display[1].off();
        g_display[2] = 0;
        g_display[3] = 0;
        g_display[2].blink();
        g_display[2].clearFlags(TUBE_FLAG_RIGHT_DOT);
        g_display[3].blink();
        g_display[3].clearFlags(TUBE_FLAG_RIGHT_DOT);
        g_display[4].off();
        g_display[5].off();
    }
    else if (s_colorProfile == 0xFF)
    {
        g_display[0].off();
        g_display[1] = (s_color.r >> 5);
        g_display[2] = (s_color.g >> 5);
        g_display[3] = (s_color.b >> 5);
        g_display[4].off();
        g_display[5].off();
        g_display[1].blink();
        g_display[1].setFlags(TUBE_FLAG_RIGHT_DOT);
        g_display[2].blink();
        g_display[2].setFlags(TUBE_FLAG_RIGHT_DOT);
        g_display[3].blink();
        g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
    }
    else
    {
        g_display[0].off();
        g_display[1].off();
        g_display[2] = (s_brightness >> (NIXIE_BRIGHTNESS_BITS - 3));
        g_display[3] = s_colorProfile + 1;
        g_display[2].blink();
        g_display[2].setFlags(TUBE_FLAG_RIGHT_DOT);
        g_display[3].blink();
        g_display[3].clearFlags(TUBE_FLAG_RIGHT_DOT);
        g_display[4].off();
        g_display[5].off();
    }
}

static void acceptColorProfile()
{
    g_savedSettings.colorProfile = s_colorProfile;
    g_savedSettings.highlightColor = s_color;
    g_savedSettings.highlightOn    = s_colorOn;
    g_savedSettings.highlightBrightness = s_brightness;
}


static void updateColor()
{
    if (s_colorOn)
    {
        g_highlight.brightness(s_brightness);
        if (s_colorProfile == 0xFF)
            g_highlight.rgb(s_color);
        else
            g_highlight.rgb(colorProfiles[s_colorProfile]);
    }
    else
    {
        g_highlight.off();
    }
}


void colorSetupEventFunction(SNixieEvent &event)
{
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {
        rejectSound();
        g_display.clearFlags();
        g_display.scrollOn();
        updateHighlight( !g_savedSettings.nightMode, g_sensor.brightness() );
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_5) )
    {
        rejectSound();
        g_display.clearFlags();
        g_display.scrollOn();
        updateHighlight( !g_savedSettings.nightMode, g_sensor.brightness() );
        NixieOs::popTask();
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_LONG_HOLD) && (event.param == BUTTON_1) )
    {
        acceptColorProfile();
        acceptSound();
        g_savedSettings.write(0x0);
        g_display.clearFlags();
        g_display.scrollOn();
        NixieOs::popTask();
        return;
    }

    /* If user sets up custom rgb color */
    if (s_colorProfile == 0xFF)
    {
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_2))
        {
            s_color.r = (s_color.r + 32) & 0xE0;
            NixieOs::resetTimer(0);
            updateColor();
        }
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_3))
        {
            s_color.g = (s_color.g + 32) & 0xE0;
            NixieOs::resetTimer(0);
            updateColor();
        }
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_4))
        {
            s_color.b = (s_color.b + 32) & 0xE0;
            NixieOs::resetTimer(0);
            updateColor();
        }
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1))
        {
            s_colorOn = false;
            s_colorProfile = 0;
            NixieOs::resetTimer(0);
            updateColor();
        }
    }
    else
    {
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1))
        {
            if (false == s_colorOn)
            {
                s_colorOn = true;
            }
            else
            {
                s_colorProfile++;
                if (s_colorProfile >= sizeof(colorProfiles)/sizeof(SNixieColor))
                {
                    s_colorProfile = 0xFF;
                }
            }
            updateColor();
            NixieOs::resetTimer(0);
        }
        if ((event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_2))
        {
            s_brightness += NIXIE_MAX_BRIGHTNESS/8;
            if (s_brightness > NIXIE_MAX_BRIGHTNESS)
            {
                s_brightness = NIXIE_MAX_BRIGHTNESS/8;
            }
            NixieOs::resetTimer(0);
            updateColor();
        }
    }
}
