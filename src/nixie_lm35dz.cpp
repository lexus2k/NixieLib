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

#include "nixie_lm35dz.h"
#include "nixieos.h"
#include "nixie_types.h"

int16_t readLm35dzCelsius(uint8_t pin)
{
    //gets and prints the raw data from the lm35
    int  value;
    // wait for ADC interrupt to happen
    do {
      value = nixieAnalogRead(pin);
    } while (value == ADC_IN_PROGRESS);
    long temp = value;
    //converts raw data into degrees celsius and prints it out
    // 500mV/1024=.48828125
    temp = temp * (50000/16) / (1024/16);
    return temp;
}


