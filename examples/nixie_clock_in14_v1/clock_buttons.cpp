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

#include "nixie_types.h"
#include "clock_buttons.h"
#include "clock_events.h"
#include "nixie_clock.h"
#include "clock_nightmode.h"
#include "nixieos.h"


#ifdef NIXIE_CLOCK_V1
                                             //BTN1  BTN2  BTN3   BTN4   1+3   2+4
const int16_t buttons[BUTTON_MAX_NUM]    =  { 752,   542,  362,   188,   635,  300};
#else
                                             //BTN1  BTN2  BTN3  BTN4  BTN5  1+3  
const int16_t buttons[BUTTON_MAX_NUM]    =  { 781,   595,  439,  297,  156,  708};
                                            //928    830   760   700   150/660, 
#endif

const byte    buttonsPin                 =  A1;


NixieAnalogButtons   g_buttons(buttonsPin, buttons, BUTTON_MAX_NUM );

static bool s_longHoldSent;

static void onButtonDown(uint8_t id, uint16_t timeDeltaMs)
{
    if (!isNightSilence())
    {
        g_audioClient.action(ACTION_CLICK);
    }
    kickNightMode();
    s_longHoldSent = false;
}

static void onButtonUp(uint8_t id, uint16_t timeDeltaMs)
{
    if (isNightSleeping())
    {
        NixieOs::sendEvent(CLOCK_EVENT_WAKE_UP, 0);
        return;
    }
    if (timeDeltaMs < 400)
    {
        NixieOs::sendEvent(CLOCK_EVENT_BUTTON_SHORT, id);
    }
}

static void onButtonHold(uint8_t id, uint16_t timeDeltaMs)
{
    if ((timeDeltaMs > 1000) && (s_longHoldSent == false))
    {
        NixieOs::sendEvent(CLOCK_EVENT_BUTTON_LONG_HOLD, id);
        g_buttons.disableUpAction();
        s_longHoldSent = true;
    }
}


void      initButtons()
{
    g_buttons.onButtonDown(&onButtonDown);
    g_buttons.onButtonUp(&onButtonUp);
    g_buttons.onButtonHold(&onButtonHold);
    g_buttons.init();
}


