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
#ifndef _NIXIEOS_BOOSTER_H_
#define _NIXIEOS_BOOSTER_H_

#include <nixie_types.h>

/* Uncomment line below if you need to disable boosting */
// #define NIXIE_BOOSTER_DISABLE

const int16_t ADC_IN_PROGRESS   =   -1;

namespace NixieOs
{

#if defined(__AVR_ATmega328P__) && !defined(NIXIE_BOOSTER_DISABLE)

/**
 * @brief sets output pin to low level signal
 *
 * sets output pin to low level signal
 *
 * @param pin - pin number to set
 */
static inline void pinLow(uint8_t pin)
{
    if (pin < 8) PORTD &= ~(1 << pin);
    else         PORTB &= ~(1 << (pin & 0x07));
}


/**
 * @brief sets output pin to high level signal
 *
 * sets output pin to high level signal
 *
 * @param pin - pin number to set
 */
static inline void pinHigh(uint8_t pin)
{
    if (pin < 8) PORTD |= (1 << pin);
    else         PORTB |= (1 << (pin & 0x07));
}


/**
 * @brief sets output pin to specified level
 *
 * sets output pin to specified level. This is
 * fast version of digitalWrite.
 *
 * @param pin - pin number to set
 * @param level - HIGH or LOW
 */
static inline void digitalWrite(uint8_t pin, uint8_t level)
{
    if (level) pinHigh(pin); else pinLow(pin);
}

/**
 * @brief Reads ADC value on specified pin
 *
 * Reads ADC value on specified pin. This is
 * fast version of standard ::analogRead().
 *
 * @param pin - pin number to read
 * @return ADC_IN_PROGRESS - means that ADC conversion is not complete yet.
 *         Positive value - ADC value read.
 * @note if function returns ADC_IN_PROGRESS, just call it once again when you're ready.
 */
int analogRead(uint8_t pin);

#elif (defined(__AVR_ATtiny25__) || \
     defined(__AVR_ATtiny45__) || \
     defined(__AVR_ATtiny85__)) && \
    !defined(NIXIE_BOOSTER_DISABLE)

/**
 * @brief sets output pin to low level signal
 *
 * sets output pin to low level signal
 *
 * @param pin - pin number to set
 */
static inline void pinLow(uint8_t pin)
{
    PORTB &= ~(1 << pin);
}


/**
 * @brief sets output pin to high level signal
 *
 * sets output pin to high level signal
 *
 * @param pin - pin number to set
 */
static inline void pinHigh(uint8_t pin)
{
    PORTB |= (1 << pin);
}


/**
 * @brief sets output pin to specified level
 *
 * sets output pin to specified level. This is
 * fast version of digitalWrite.
 *
 * @param pin - pin number to set
 * @param level - HIGH or LOW
 */
static inline void digitalWrite(uint8_t pin, uint8_t level)
{
    if (level) pinHigh(pin); else pinLow(pin);
}


/**
 * @brief Reads ADC value on specified pin
 *
 * Reads ADC value on specified pin. This is
 * fast version of standard ::analogRead().
 *
 * @param pin - pin number to read
 * @return ADC_IN_PROGRESS - means that ADC conversion is not complete yet.
 *         Positive value - ADC value read.
 * @note if function returns ADC_IN_PROGRESS, just call it once again when you're ready.
 */
static inline int analogRead(uint8_t pin)
{
    return ::analogRead(pin);
}

#else /* Nixie Booster disabled */

/**
 * @brief sets output pin to low level signal
 *
 * sets output pin to low level signal
 *
 * @param pin - pin number to set
 */
static inline void pinLow(uint8_t pin)
{
    ::digitalWrite(pin, LOW);
}

/**
 * @brief sets output pin to high level signal
 *
 * sets output pin to high level signal
 *
 * @param pin - pin number to set
 */
static inline void pinHigh(uint8_t pin)
{
    ::digitalWrite(pin, HIGH);
}


/**
 * @brief sets output pin to specified level
 *
 * sets output pin to specified level. This is
 * fast version of digitalWrite.
 *
 * @param pin - pin number to set
 * @param level - HIGH or LOW
 */
static inline void digitalWrite(uint8_t pin, uint8_t level)
{
    ::digitalWrite(pin, level);
}

/**
 * @brief Reads ADC value on specified pin
 *
 * Reads ADC value on specified pin. This is
 * fast version of standard ::analogRead().
 *
 * @param pin - pin number to read
 * @return ADC_IN_PROGRESS - means that ADC conversion is not complete yet.
 *         Positive value - ADC value read.
 * @note if function returns ADC_IN_PROGRESS, just call it once again when you're ready.
 */
static inline int analogRead(uint8_t pin)
{
    return ::analogRead(pin);
}

#endif

}

#endif
