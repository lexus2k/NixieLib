/*
    Copyright (C) 2017 Alexey Dynda

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

#ifndef _NIXIE_HTU21_H_
#define _NIXIE_HTU21_H_

#include "nixie_types.h"

#define HTU21_I2CADDR       0x40
/* no hold master mode */
#define HTU21_READTEMP      0xF3
/* no hold master mode */
#define HTU21_READHUM       0xF5
#define HTU21_WRITEREG      0xE6
#define HTU21_READREG       0xE7
#define HTU21_RESET         0xFE

/** 
 *  Class for using HTU21 temperature and humidity sensor
 */
class NixieHTU21
{
public:
    NixieHTU21() { m_no_device = false; };

    /**
     * @brief Initializes HTU21 sensor
     * Initializes HTU21 sensor. It takes about 15ms to complete initialization.
     */
    void init();

    /**
     * @brief Returns temperature in hundreds of Celcius.
     * @return temperature value in hundreds of Celcius.
     *         -32000 in case of error.
     */
    int16_t getTemperature();

    /**
     * @brief Returns humidity in percents.
     * @return humidity value in percents.
     *         -32000 in case of error
     */
    int16_t getHumidity();
private:
    bool m_no_device;
};

#endif

