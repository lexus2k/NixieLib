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

#ifndef _CLOCK_EVENTS_H_
#define _CLOCK_EVENTS_H_

#include "nixieos.h"
#include "nixie_types.h"

enum
{
    CLOCK_EVENT_BUTTON_SHORT      = NIXIEOS_USER_EVENTS,
    CLOCK_EVENT_BUTTON_LONG_HOLD,
    CLOCK_EVENT_WAKE_UP,
    CLOCK_EVENT_BRIGHTNESS_CHANGED,
};

#endif

