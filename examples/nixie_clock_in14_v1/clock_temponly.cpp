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
#include "clock_temponly.h"
#include "clock_generic.h"
#include "clock_events.h"
#include "nixie_clock.h"
#include "nixie_lm35dz.h"
#include "clock_buttons.h"
#include "clock_fine_tuning.h"

NixieHTU21 g_htu21;

static inline void displayTempHtu21()
{
    g_display.powerOff();
    int16_t temp = g_htu21.getTemperature();
    g_display.powerOn();
    if (temp < 0) temp = 0; /* TODO: Sadly we have no minus char */
    g_display.clearFlags();
    g_display[0].scrollOff(0);
    g_display[1].scrollOff(0);
    g_display[2] = (temp/100) / 10;
    g_display[3] = (temp/100) % 10;
    g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[4] = ((temp % 100) / 10);
    g_display[5] = ((temp % 100) % 10);
}


static inline void displayTempLM35DZ()
{
    int16_t temp = readLm35dzCelsius(A2);
    if (temp < 0) temp = 0; /* TODO: Sadly we have no minus char */
    g_display.clearFlags();
    g_display[0].scrollOff(0);
    g_display[1].scrollOff(0);
    g_display[2] = (temp/100) / 10;
    g_display[3] = (temp/100) % 10;
    g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[4] = ((temp % 100) / 10);
    g_display[5] = ((temp % 100) % 10);
}

static inline void displayTempDS3232()
{
    int16_t temp = g_rtc.getTemp(); // TODO: Replace with ADC from LM35
    if (temp < 0) temp = 0;         /* TODO: Sadly we have no minus char */
    g_display.clearFlags();
    g_display[0].scrollOff();
    g_display[1].scrollOff();
    g_display[2] = (temp >> 2) / 10;
    g_display[3] = (temp >> 2) % 10;
    g_display[3].setFlags(TUBE_FLAG_RIGHT_DOT);
    g_display[4] = ((temp & 0x3) * 25 / 10);
    g_display[5].scrollOff();
}


void temperatureOnlyEnterFunction()
{
    NixieOs::startTimer(0, TEMPERATURE_SCREEN_TIMEOUT_MS);
    g_display.scrollForward();
    displayTempHtu21();
}


void temperatureOnlyStateFunction()
{
    genericStateFunction();
}


void temperatureOnlyEventFunction(SNixieEvent &event)
{
    if ( genericEventFunction(event) )
    {
        return;
    }
    if ( (event.event == CLOCK_EVENT_BUTTON_SHORT) && (event.param == BUTTON_1) )
    {
        g_display.scrollOn();
        NixieOs::switchTask(CLOCK_STATE_TIME_AND_DATE);
    }
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0 )
    {  
        g_display.moveRight();
        g_display.clearFlags();
        NixieOs::switchTask(CLOCK_STATE_TIME_ONLY);
    }
}
