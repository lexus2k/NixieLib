/*
    Copyright (C) 2016-2017 Alexey Dynda

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

#ifndef _NIXIEOS_TIME_H_
#define _NIXIEOS_TIME_H_

#include "nixieos_types.h"

/** This variable contains lower 16-bits of microseconds counter. For internal use by Nixie Library only! */
#define g_nixieUs   ((uint16_t)g_nixieUsEx)
/** This variable contains lower 16-bits of milliseconds counter. For internal use by Nixie Library only! */
#define g_nixieMs   ((uint16_t)g_nixieMsEx)

/** This variable contains full  32-bits of microseconds counter. For internal use by Nixie Library only! */
extern uint32_t  g_nixieUsEx;
/** This variable contains full  32-bits of milliseconds counter. For internal use by Nixie Library only! */
extern uint32_t  g_nixieMsEx;

/**
 * Describes time
 */
typedef struct 
{
    /// hours
    uint8_t hours;
    /// minutes
    uint8_t minutes;
    /// seconds
    uint8_t seconds;  
} NixieOsTime;


namespace NixieOs
{

/*void setTimeEx(uint8_t hour, uint8_t minutes, uint8_t seconds);

void setTime(NixieOsTime timeRec);

void getTime(NixieOsTime *timeRec);*/

}

#endif


