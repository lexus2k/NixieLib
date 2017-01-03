= Nixie Library for Arduino =

With this library you can easy develop software for Nixie Clock and create
interesting sound and visual effects. The library contains all required
modules to communicate with different peripherals hardware (RTC, sensors,
tube drivers). The library was tested on Attiny85, Attiny45, Atmega168
and Atmega328p controllers. Nixie Library includes faster digitalWrite,
analogRead functions implementations (nixiePinLow, nixiePinHigh,
nixieAnalogRead) for Atmega328p controllers. If you have questions, please
contact via email.

The library supports the following devices:
 * DS3231, DS3232 RTC chips
 * Piezo buzzer audio output
 * Analog buttons
 * Nixie Tubes (any: IN-14, IN-12, etc. )
 * GL 5528 lightsensor (most popular light sensor on Arduino platforms)
 * K155ID1 nixie drivers
 * Mono-LEDs
 * RGB-LEDs (both PWM controlled an Software controlled)
 * LM35DZ temperature sensor

For more information about this library please visit
https://github.com/lexus2k/nixielib.
If you found any problem, please report to Issues section.

== License ==

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

