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

#ifndef _NIXIE_DS3232_H_
#define _NIXIE_DS3232_H_

#include <nixie_types.h>

/**
 * Ds3231 class supports Ds3231 and Ds3232 chips.
 * The class encapsulates the work with RTC chip,
 * allowing to set and read time.
 * Class uses Wire library.
 */
class Ds3231
{
public:
    /**
     * All public fields are in internal DS3231 representation: XXXX YYYY
     * where XXXX bits is high decimal digit and YYYY is low decimal digit. 
     * 00101001 means 29 seconds (or minutes)
     */
    uint8_t          m_seconds;
    uint8_t          m_minutes;
    uint8_t          m_hours;
    uint8_t          m_day_of_week;
    uint8_t          m_day;
    uint8_t          m_month;
    uint8_t          m_year;

    /**
     * Initializes Ds3231 class state. Should be called in setup
     */
    void init();

    Ds3231() {};

    /**
     * Reads date and time from the chip
     */
    void    getDateTime();

    /**
     * Writes date and time to the chip
     */
    void    setDateTime();

    /**
     * Writes only date to the chip. The time state is preserved.
     */
    void    setDate();

    /**
     * Writes only time to the chip. The date state is preserved.
     */
    void    setTime();
    
    /** 
     * Returns temp in celsius times four.
     * For example, 100 means 25.0C
     */
    int16_t getTemp();

    /**
     * Increments date and time variables by 1 second
     * The function doesn't changes state of RTC chip.
     */
    Ds3231& operator  ++();

    /**
     * Converts integer value to internal DS3231 representation.
     * integer value 12 will be converted to 00010010 = 0x12
     */
    static uint8_t toInternal(uint8_t d);

    /**
     * Converts internal value representation to decimal:
     * internal value 0x22 will be converted to integer 22.
     */
    static uint8_t toDecimal(uint8_t i);

    /**
     * Returns number of minutes passed since midnight 00:00.
     * @param decimal - if false, passed hours and minutes values will be
     *                  converted to decimal
     * @return number of minutes since midnight
     */
    static int16_t toMinutes(uint8_t hours, uint8_t minutes, bool decimal);

    /**
     * Returns minimum time delta between 2 timestamps.
     * For example, if timestamps are 60 and 120, the function will return 60.
     * If timestamps are 120 and 60, the function will return 60
     * If timestamps are 22*60 and 60, the function will return 180.
     */
    static int16_t timeDelta(int16_t min1, int16_t min2);
private:
    static const int I2C_ADDR_AT24C32 = 0x57;
    static const int I2C_ADDR_DS3231  = 0x68;
};

#endif /* _NIXIE_RTC_H_ */

