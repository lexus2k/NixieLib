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

#ifndef _CLOCK_NIGHTMODE_H_
#define _CLOCK_NIGHTMODE_H_

#include "nixie_types.h"

void nightModeState();
void wakeUpClock();
void changeNightMode();
bool isNightSilence();
bool isNightSleeping();
void kickNightMode();

#endif /* _CLOCK_NIGHTMODE_H_ */

