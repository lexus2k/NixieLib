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

#include "nixie_library.h"
#include "nixie_tube.h"
#include "nixie_booster.h"


void  NixieTube::init()
{
    if (m_pin != 0xFF)
    {
        pinMode( m_pin, OUTPUT );
    }
    if (m_driver != nullptr)
    {
        m_driver->init();
    }
    m_brightness = NIXIE_MAX_BRIGHTNESS;
    m_tempBrightness = NIXIE_MAX_BRIGHTNESS;
}


bool NixieTube::update()
{
    /* To reduce load of uC, during active period perform calculations only during
       updating pwm state
    */
    if (g_nixieUs - m_uts < CHANGE_INTERVAL)
    {
        if (g_nixieUs - m_uts < m_impulseWidth )
        {
            if ( (m_pinState & PIN_STATE_POWERED) && !(m_pinState & PIN_STATE_PWM_HIGH ) )
            {
                m_pinState |= PIN_STATE_PWM_HIGH;
                updatePinState();
            }
        }
        else if (m_pinState & PIN_STATE_PWM_HIGH)
        {
            m_pinState &= ~(PIN_STATE_PWM_HIGH);
            updatePinState();
        }
        return false;
    }
    m_uts = g_nixieUs;

    switch (m_state)
    {
    case TUBE_OFF:
        m_mts = g_nixieMs;
        break;
    case TUBE_SMOOTH_OFF:
        if (m_tempBrightness == 0)
        {
            m_state = TUBE_OFF;
        }
        else
        {
            updateDimming();  
        }
        break;
    case TUBE_NORMAL:
        m_digit = m_value;
        updateDimming();
        break;
    case TUBE_BLINK_ON:
        if ((g_nixieMs & (BLINK_INTERVAL - 1)) <= (BLINK_INTERVAL >> 1))
        {
            m_state = TUBE_BLINK_OFF;
        }
        m_digit = m_value;
        updateDimming();
        break;
    case TUBE_BLINK_OFF:
        if ((g_nixieMs & (BLINK_INTERVAL - 1)) > (BLINK_INTERVAL >> 1))
        {
            m_state = TUBE_BLINK_ON;
        }
        updateDimming();
        break;
    case TUBE_SCROLL_OFF:
        {
            uint16_t delta = g_nixieMs - m_mts;
            if (delta >= SCROLL_INTERVAL)
            {
                m_mts += SCROLL_INTERVAL;
                if (++m_digit == 10)
                {
                    m_digit = 0;
                }
            }
            if (0 != m_delayedScroll)
            {
                if (delta < m_dimmingTs )
                {
                    m_dimmingTs -= delta;
                }
                else
                {
                    m_mts = g_nixieMs;
                    m_dimmingTs = m_mts;
                    m_delayedScroll = 0;
                }
            }
            else
            {
                updateDimming();
            }
            if (m_tempBrightness == 0)
            {
                m_state = m_next;
            }
        }
        break;
    case TUBE_SCROLL_WRAP:
    case TUBE_SCROLL_FORWARD:
        if (g_nixieMs - m_mts >= SCROLL_INTERVAL)
        {
            m_mts += SCROLL_INTERVAL;
            if (m_delayedScroll > 0) m_delayedScroll--;
            if (++m_digit == 10)
            {
                m_digit = 0;
            }
            if ((m_digit == m_value) && (m_delayedScroll == 0))
            {
                m_mts = g_nixieMs;
                m_state = m_next;
            }
        }
        updateDimming();
        break;
    case TUBE_SCROLL_BACK:
        if (g_nixieMs - m_mts >= SCROLL_INTERVAL)
        {
             m_mts += SCROLL_INTERVAL;
             if (m_delayedScroll > 0) m_delayedScroll--;
             if (m_digit-- == 0)
             {
                 m_digit = 9;
             }
             if ((m_digit == m_value) && (m_delayedScroll == 0))
             {
                 m_state = m_next;
             }
        }
        updateDimming();
        break;
    case TUBE_SCROLL_ON:
        {
            uint16_t delta = g_nixieMs - m_mts;
            if (delta < m_dimmingTs )
            {
                m_dimmingTs -= delta;
                m_mts = g_nixieMs;
            }
            else
            {
                m_dimmingTs = g_nixieMs;
                scrollForward();
            }
        }
        break;
    default:
       /* for unknown state return OFF */
        m_digit = 0xFF;
        break;
    }
    return true;
}


void  NixieTube::anodOff()
{
    m_pinState &= ~(PIN_STATE_POWERED | PIN_STATE_PWM_HIGH); 
    updatePinState();
}


void  NixieTube::anodOn()
{
    m_pinState |= (PIN_STATE_POWERED | PIN_STATE_PWM_HIGH);
    m_uts     = g_nixieUs;
    updatePinState();
}


void  NixieTube::updatePinState()
{
    // Set digit code for new digit to display.
    if ( (m_pinState & PIN_STATE_PWM_HIGH) && isBurning() )
    {
        m_driver->switchPin( m_map[m_digit] );
        m_driver->setExtendedPins( m_flags );
        // Turn ON next bulb
        if ( m_pin != 0xFF )
        {
            nixiePinHigh( m_pin );
        }
    }
    else
    {
        if ( m_pin != 0xFF )
        {
            nixiePinLow( m_pin );
        }
    }
}


void  NixieTube::updateDimming  ()
{
    if (g_nixieMs - m_dimmingTs >= DIMMING_INTERVAL)
    {
        m_dimmingTs += DIMMING_INTERVAL;
        if ((m_state == TUBE_SMOOTH_OFF) || (m_state == TUBE_SCROLL_OFF))
        {
            if (m_tempBrightness > 0) setActiveBrightness( m_tempBrightness - 1 );
        }
        else if (m_tempBrightness != m_brightness)
        {
            setActiveBrightness( m_tempBrightness + (m_tempBrightness < m_brightness ? +1 : -1) );
        }
    }
}


void NixieTube::scrollOn()
{
    scrollOn(m_value * SCROLL_INTERVAL);
}


void NixieTube::scrollOn(uint16_t msDelay)
{
    if (m_state == TUBE_OFF)
    {
        m_dimmingTs = msDelay;
        m_mts = g_nixieMs;
        m_state = TUBE_SCROLL_ON;
    }
    else
    {
        scrollForward();
    }
}

void  NixieTube::scrollForward(ENixieTubeState next)
{
    m_state = TUBE_SCROLL_FORWARD;
    m_delayedScroll = 10;
    m_next = next;
    m_digit = m_value;
    m_mts = g_nixieMs;
}

void  NixieTube::scrollBack(ENixieTubeState next)
{
    m_state = TUBE_SCROLL_BACK;
    m_delayedScroll = 10;
    m_next = next;
    m_digit = m_value;
    m_mts = g_nixieMs;
};


void  NixieTube::scrollOff()
{
    scrollOff( (m_value >> 1) * SCROLL_INTERVAL ); 
}

void  NixieTube::scrollOff(uint16_t msDelay)
{
    if ((m_state!=TUBE_OFF) && (m_state != TUBE_SCROLL_OFF))
    {
        m_delayedScroll = 1;
        m_dimmingTs = msDelay;
        m_next = TUBE_OFF;
        m_state = TUBE_SCROLL_OFF;
        m_mts = g_nixieMs;
    }
};

/**
 * Copies state of source tube to new tube.
 * All timer related variables are preserved.
 */
void NixieTube::operator =(const NixieTube &tube)
{
    /* Copy displayed value and current state of the tube digit */
    m_value = tube.m_value;
    m_digit = tube.m_digit;
    /* Copy brightness */
    m_brightness = tube.m_brightness;
    m_impulseWidth = tube.m_impulseWidth;
    m_tempBrightness = tube.m_tempBrightness;
    /* Copy state variables */
    m_state = tube.m_state;
    m_next = tube.m_next;
    /* Copy flags from the source tube */
    m_flags = tube.m_flags;
    m_delayedScroll = tube.m_delayedScroll;
    m_mts = tube.m_mts;

    /* We do not copy timer related variables */
    /* Each tube object must calculate them itself.
     * Otherwise, it breaks tube effects. */
}


static const uint32_t s_newInterval = ( (uint32_t)CHANGE_INTERVAL * DISPLAY_BRIGHTNESS_RANGE ) / (uint32_t)NIXIE_MAX_BRIGHTNESS;
static const uint32_t s_minChangeInterval = (uint32_t)CHANGE_INTERVAL - s_newInterval;

void  NixieTube::setActiveBrightness( uint8_t brightness )
{
    m_tempBrightness = brightness;
    m_impulseWidth = (uint16_t)(( s_minChangeInterval + s_newInterval * (uint32_t)brightness) >> NIXIE_BRIGHTNESS_BITS );
}

