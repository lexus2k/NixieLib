/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of Nixie Library.

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
 This file contains cross-platform definition of the types,
 used by Nixie Library.

 @file
 @brief Nixie Library Types
*/


#ifndef _NIXIE_TYPES_H_
#define _NIXIE_TYPES_H_

#include <arduino.h>
#include <avr/pgmspace.h>

/**
 *  SNixieColor describes RGB-color model of Nixie Library
 */
typedef struct SNixieColor
{
    /** Red component value in the range [0..255] */
    uint8_t r;
    /** Green component value in the range [0..255] */
    uint8_t g;
    /** Blue component value in the range [0..255] */
    uint8_t b;
} SNixieColor;

#endif

