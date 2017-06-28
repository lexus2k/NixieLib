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

#ifndef _NIXIE_K155ID1_H_
#define _NIXIE_K155ID1_H_

#include "nixie_types.h"
#include "nixie_driver.h"
    
/*********************************************************************
 *                      K155 DRIVER MODULE
 *                      
 *     The main purpose of this driver is to control K155ID1 chip
 *     by setting binary input lines corresponding to specified
 *     digit.
 *
 *********************************************************************/

/**
 * @brief Transforms digit to binary code to control K155ID1 driver
 *
 * Standard K155ID1 Nixie driver. (4 input pins, 10 outputs).
 */
class K155ID1: public NixieDriver
{
public:
    /**
     * Standard K155ID1 Nixie driver. (4 input pins, 10 outputs).
     * @param[in] pins - array of pin numbers to control K155ID1 driver
     */
    inline         K155ID1(const uint8_t pins[])
    {
        K155ID1(pins, 0);
    }

    /**
     * Standard K155ID1 Nixie driver. (4 input pins, 10 outputs).
     * additionally you can specifies pins to control separate elemets like commas.
     * @param[in] pins - array of pin numbers to control (>=4). First 4 are to control
     *              K155ID1 driver.
     * @param[in] extendedCount - number of additional pins to control
     *            each additional pin corresponds to extended flag
     * @see setExtendedPins
     * @see switchPin
     */
    inline         K155ID1(const uint8_t pins[], uint8_t extendedCount)
    {
        m_pins = pins;
        m_count = 4 + extendedCount;
    }

    /**
     *  Must be called in setup().
     */
    void           init(void);

    /**
     * Sets binary code on the K155ID1 inputs, corresponding to specified digit,
     * and changes extended control pins. 
     * @param[in] pin - output to enable in the range [0;9]. K155ID1 has 10 outputs.
     * @param[in] extendedPins - extended pins to enable
     * @note all other pins are usually become disabled
     */
    virtual void   switchPin(uint8_t pin, uint8_t extendedPins);
   
private:
    const uint8_t    *m_pins;
    uint8_t           m_count;
};

#endif
