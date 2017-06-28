/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of Nixie Os Library.

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

/**
 * This example demontrates how to use timers of NixieOs.
 */

#include <nixieos.h>

void defaultEventHandler(SNixieEvent &event)
{
    if (event.event == NIXIEOS_EVENT_TIMEOUT && event.param == 0)
    {
        Serial.print("Timer event happened");
        NixieOs::restartTimer(0);
    }
}


void setup()
{
    NixieOs::setup( nullptr );
    Serial.begin(9600);
    /* Timer event after 10 seconds. Use 0-timer of NixieOs */
    NixieOs::startTimer(0, 10000);
}

void loop()
{
    NixieOs::run();
}
