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

/**
 * This example demontrates how to read htu21 sensor. According to datasheet
 * power supply for HTU21 should be 3.3V. Thus, make sure that your Arduino
 * is powered from 3.3V or you use 5V<->3.3V level converters for I2C bus.
 *
 * HTU21 sensor is connected to I2C bus, and is powered from 3.3V supply.                
 */

#include <nixie_library.h>
#include <nixie_htu21.h>

static NixieHTU21 s_htu21;


void onButtonDown(uint8_t id, uint16_t timeDeltaMs)
{
    Serial.print("Pressed button: ");
    Serial.println(id + 1); /* indexes are zero-based */
}


void setup()
{
    Serial.begin(9600);
    s_htu21.init();
}

void loop()
{
    NixieLibrary::update();
    int16_t temp = s_htu21.getTemperature();
    Serial.print("Temperature: ");
    Serial.println(temp/100); /* Print temperature in Celsius */
    int16_t humidity = s_htu21.getHumidity();
    Serial.print("Humidity: ");
    Serial.println(humidity); /* Print humidity in percents */
    delay(2000);
}
