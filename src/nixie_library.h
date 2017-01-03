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
 This file contains NixieLibrary class, which control global states of NixieLibrary

 @file
 @brief Nixie Library - global settings of the library.
*/

#ifndef _NIXIE_LIBRARY_H_
#define _NIXIE_LIBRARY_H_

#include "nixie_types.h"

/** This variable contains lower 16-bits of microseconds counter. For internal use by Nixie Library only! */
extern uint16_t  g_nixieUs;
/** This variable contains lower 16-bits of milliseconds counter. For internal use by Nixie Library only! */
extern uint16_t  g_nixieMs;

/**
 * NixieLibrary class contains static methods to be called in loop cycle and setup library
 */
class NixieLibrary
{
public:

    /**
     * @brief updates ms and us counters of NixieLibrary
     * Call this method in the main loop.
     * @param[in] ms - time in milliseconds returned by millis()
     * @param[in] us - time in microseconds returned by micros()
     * @note if you don't want to call millis() and micros() by yourself
     *       please, use update() method without parameters
     */
    static inline void update(uint32_t ms, uint32_t us)
    {
       g_nixieMs = (uint16_t)ms;
       g_nixieUs = (uint16_t)us;
    };

    /**
     * @brief updates ms and us counters of NixieLibrary
     * Call this method in the main loop.
     * This method uses standard millis() and micros() functions.
     */
    static inline void update()
    {
       g_nixieMs = millis();  
       g_nixieUs = micros();
    };

};


#endif


