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

#ifndef _CLOCK_STATES_H_
#define _CLOCK_STATES_H_

typedef enum
{
    CLOCK_STATE_TIME_ONLY,
    CLOCK_STATE_DATE_ONLY,
    CLOCK_STATE_TEMP_ONLY,
    CLOCK_STATE_TIME_AND_DATE,
    CLOCK_STATE_TIME_SETUP,
    CLOCK_STATE_DATE_SETUP,
    CLOCK_STATE_COLOR_SETUP,
    CLOCK_STATE_BRIGHTNESS_SETUP,
    CLOCK_STATE_ALARM_SETUP,
    CLOCK_STATE_SOUND_SETUP,
    CLOCK_STATE_ALARM_ACTIVE,
    CLOCK_SHUTDOWN,
    CLOCK_DEBUG,
    CLOCK_DIAG_STATE_TUBES,
    CLOCK_DIAG_STATE_LIGHT_SENSOR,
    CLOCK_STATE_MAX,
    CLOCK_STATE_INVALID = 0xFF,
} EClockState;

#endif /* _CLOCK_STATES_H_ */

