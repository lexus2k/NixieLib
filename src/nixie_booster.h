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
 * This module contains improved ADC/Digital pin service functions 
 */
#ifndef _NIXIE_BOOSTER_H_
#define _NIXIE_BOOSTER_H_

#include <nixie_types.h>

/* Uncomment line below if you need to disable boosting */
// #define NIXIE_BOOSTER_DISABLE

const int16_t ADC_IN_PROGRESS   =   -1;

#if defined(__AVR_ATmega328P__) && !defined(NIXIE_BOOSTER_DISABLE)

inline void __nixiePinLow(uint8_t pin)
{
    if (pin < 8) PORTD &= ~(1 << pin);
    else         PORTB &= ~(1 << (pin & 0x07));
}

inline void __nixiePinHigh(uint8_t pin)
{
    if (pin < 8) PORTD |= (1 << pin);
    else         PORTB |= (1 << (pin & 0x07));
}

#define nixiePinLow(pin)                __nixiePinLow(pin)
#define nixiePinHigh(pin)               __nixiePinHigh(pin)
#define nixieDigitalWrite(pin, level)   { if (level) __nixiePinHigh(pin); else __nixiePinLow(pin); }

int nixieAnalogRead(uint8_t pin);

#elif (defined(__AVR_ATtiny25__) || \
     defined(__AVR_ATtiny45__) || \
     defined(__AVR_ATtiny85__)) && \
    !defined(NIXIE_BOOSTER_DISABLE)

inline void __nixiePinLow(uint8_t pin)
{
    PORTB &= ~(1 << pin);
}

inline void __nixiePinHigh(uint8_t pin)
{
    PORTB |= (1 << pin);
}

#define nixiePinLow(pin)                __nixiePinLow(pin)
#define nixiePinHigh(pin)               __nixiePinHigh(pin)

/* implementation of standard functions */
#define nixieDigitalWrite(pin, level)   { if (level) __nixiePinHigh(pin); else __nixiePinLow(pin); }
#define nixieAnalogRead(pin)            analogRead(pin)

#else /* Nixie Booster disabled */

#define nixiePinLow(pin)                digitalWrite(pin, LOW)  
#define nixiePinHigh(pin)               digitalWrite(pin, HIGH)

/* implementation of standard functions */
#define nixieDigitalWrite(pin, level)   digitalWrite(pin, level)
#define nixieAnalogRead(pin)            analogRead(pin)

#endif

/**
 * Inits internal data of booster module
 */
void initNixieBooster();

#endif
