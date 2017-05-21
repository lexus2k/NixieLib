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

#ifndef _NIXIE_DECODER_H_
#define _NIXIE_DECODER_H_

#include "nixie_types.h"
    
/*********************************************************************
 *     The main purpose of the NixieDriver is to control the circutes
 *     connected to the nixie tubes.
 *********************************************************************/

/**
 * Base class for all drivers to control Nixie tubes cathodes.
 */
class NixieDriver
{
public:
    /**
     * Initializes Nixie Driver
     */
    virtual void init() = 0;

    /**
     * Enables specified output pin on Nixie driver
     * @param[in] pin - pin to enable
     * @note all other pins are usually become disabled
     */
    virtual void switchPin(uint8_t pin) = 0;

    /**
     * Changes extended control pins. 
     * Actual implementation depends on specific driver.
     */
    virtual void setExtendedPins(uint8_t value) = 0;
};

#endif
