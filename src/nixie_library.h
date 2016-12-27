/*
    Copyright (C) 2016 Alexey Dynda

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
 @brief Nixie Library - global.
*/

#ifndef _NIXIE_LIBRARY_H_
#define _NIXIE_LIBRARY_H_

#include "nixie_types.h"

extern uint16_t  g_nixieUs;
extern uint16_t  g_nixieMs;

class NixieLibrary
{
public:
    static inline void update(uint32_t ms, uint32_t us)
    {
       g_nixieMs = (uint16_t)ms;
       g_nixieUs = (uint16_t)us;
    };

    static inline void update()
    {
       g_nixieMs = millis();  
       g_nixieUs = micros();
    };

};


#endif


