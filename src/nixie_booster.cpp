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

#include <arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <nixie_booster.h>

#if defined(__AVR_ATmega328P__) && !defined(NIXIE_BOOSTER_DISABLE)

#define MAX_ADC_PINS               9
#define ADC_INVALID_VALUE         -2

extern uint8_t analog_reference;
volatile static int adcValue[MAX_ADC_PINS];
volatile static uint8_t activePin = 0;
 
/*ADC Conversion Complete Interrupt Service Routine (ISR)*/
ISR(ADC_vect)
{
    adcValue[activePin] = ADCL | (ADCH << 8);     // Output ADCH to PortD
    uint8_t pin = activePin;
    do
    {
        if (++activePin >= MAX_ADC_PINS) activePin = 0;
        /* if somebody requested another pin */
        if (adcValue[activePin] == ADC_IN_PROGRESS)
        {
            /* start next conversion */
            ADMUX = (analog_reference << 6) | activePin; // 1 = DEFAULT AVCC REF
            ADCSRA |= 1<<ADSC;
            break;
        }
    } while (pin != activePin);
}

int nixieAnalogRead(uint8_t pin)
{
    if (pin >= 14) pin-=14;
    /* Wait for previous conversion to stop */
    cli();
    if (adcValue[pin] == ADC_INVALID_VALUE)
    {
        adcValue[pin] = ADC_IN_PROGRESS;
        /* If convertor is not running */
        if (!(ADCSRA & (1<<ADSC)))
        {
            /* Start processing */
            ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (0<<ADPS1);
            ADMUX = (analog_reference << 6) | pin; // 1 = DEFAULT AVCC REF
            sei();
            activePin = pin;
            ADCSRA |= 1<<ADSC;
            return ADC_IN_PROGRESS;
        }
    }
    sei();
    if (adcValue[pin] == ADC_IN_PROGRESS)
    {
        return ADC_IN_PROGRESS;
    }
    int temp = adcValue[pin];
    adcValue[pin] = ADC_INVALID_VALUE;
    return temp;
}


void initNixieBooster()
{
    for(uint8_t pin=0; pin<MAX_ADC_PINS; pin++)
    {
        adcValue[pin] = ADC_INVALID_VALUE;
    }
}


#else

void initNixieBooster()
{
}


#endif
