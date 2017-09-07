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
#include "clock_sound_setup.h"
#include "nixie_clock.h"
#include "clock_events.h"
#include "clock_sounds.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"

static uint8_t s_clockMelody;


void soundSetupEnterFunction()
{
    NixieOs::startTimer(0,SETUP_INACTIVITY_TIMEOUT_MS);
    s_clockMelody = g_savedSettings.clockMelody;
    uint8_t c = g_audioClient.getSoundCount();
    if ( c >= g_count) c = g_count - 1;
    for (uint8_t i=0; i<=c; i++)
    {
        g_display[i] = i;
        g_display[i].clearFlags(0xFF);
    }
    for (uint8_t i = c + 1 ; i < g_count; i++)
    {
        g_display[i].smoothOff();
    }
    g_display[s_clockMelody].blink();
    if (s_clockMelody > 0)
    {
        g_audioClient.sound( s_clockMelody - 1 );
    }
}


void soundSetupStateFunction()
{
}


static void acceptSoundSetup()
{
    g_savedSettings.clockMelody = s_clockMelody;
}


void soundSetupEventFunction(SNixieEvent &event)
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
        acceptSoundSetup();
        g_display.swapSmooth();
        NixieOs::popTask();
        return;
    }
    if ( event.event == CLOCK_EVENT_BUTTON_SHORT )
    {
        NixieOs::resetTimer(0);
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_2 ) )
    {
        uint8_t c = g_audioClient.getSoundCount();
        g_display[s_clockMelody].noBlink();
        if (++ s_clockMelody > c)
        {
            s_clockMelody = 0;
        }
        if (s_clockMelody > 0)
            g_audioClient.sound( s_clockMelody - 1 );
        else
            g_audioClient.stop();
        g_display[s_clockMelody].blink();
    }
}
