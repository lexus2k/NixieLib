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

#ifndef _CLOCK_BUTTONS_H_
#define _CLOCK_BUTTONS_H_

#include "nixie_buttons.h"

typedef enum
{
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
#ifndef NIXIE_CLOCK_V1
    BUTTON_5,
#endif
    BUTTON_13,
#ifdef NIXIE_CLOCK_V1
    BUTTON_24,
#endif
    BUTTON_MAX_NUM,
} EButtons;

extern    NixieAnalogButtons   g_buttons;

void      initButtons();

#endif

