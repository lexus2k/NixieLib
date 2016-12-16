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

#include "nixie_tube.h"

#define             TUBE_BLINK_FLAG     0x10
#define             TUBE_OFF_FLAG       0x20


void NixieTube::update(uint16_t ts)
{
    switch (m_state)
    {
    case TUBE_OFF:
        m_ts = ts;
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
        m_ts = ts;
        m_digit = m_value;
        updateDimming();
        break;
    case TUBE_BLINK_ON:
        if ((ts & (BLINK_INTERVAL - 1)) <= (BLINK_INTERVAL >> 1))
        {
            m_state = TUBE_BLINK_OFF;
        }
        m_digit = m_value;
        m_ts = ts;
        updateDimming();
        break;
    case TUBE_BLINK_OFF:
        if ((ts & (BLINK_INTERVAL - 1)) > (BLINK_INTERVAL >> 1))
        {
            m_state = TUBE_BLINK_ON;
        }
        m_ts = ts;
        updateDimming();
        break;
    case TUBE_SCROLL_OFF:
        if (ts - m_ts >= SCROLL_INTERVAL)
        {
            if (m_delayedScroll)
            {
                if (ts - m_ts < m_dimmingTs )
                {
                    m_dimmingTs = m_dimmingTs - (ts - m_ts);
                }
                else
                {
                    m_dimmingTs = ts;
                    m_delayedScroll = 0;
                }
            }
            m_ts += SCROLL_INTERVAL;
            if (++m_digit == 10)
            {
                m_digit = 0;
            }
            if (m_tempBrightness == 0)
            {
                m_state = m_next;
            }
        }
        if (!m_delayedScroll) updateDimming();
        break;
    case TUBE_SCROLL_WRAP:
    case TUBE_SCROLL_FORWARD:
        if (ts - m_ts >= SCROLL_INTERVAL)
        {
            m_ts += SCROLL_INTERVAL;
            if (m_delayedScroll > 0) m_delayedScroll--;
            if (++m_digit == 10)
            {
                m_digit = 0;
            }
            if ((m_digit == m_value) && (m_delayedScroll == 0))
            {
                m_state = m_next;
            }
        }
        updateDimming();
        break;
    case TUBE_SCROLL_BACK:
        if (ts - m_ts >= SCROLL_INTERVAL)
        {
             m_ts += SCROLL_INTERVAL;
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
        if (ts - m_ts < m_dimmingTs )
        {
            m_dimmingTs = m_dimmingTs - (ts - m_ts);
        }
        else
        {
            m_dimmingTs = ts;
            scrollForward();
        }
        m_ts = ts;
        break;
    default:
       /* for unknown state return OFF */
        m_digit = 0xFF;
        break;
    }
}


void  NixieTube::updateDimming  ()
{
    if (m_ts - m_dimmingTs >= DIMMING_INTERVAL)
    {
        m_dimmingTs += DIMMING_INTERVAL;
        if ((m_state == TUBE_SMOOTH_OFF) || (m_state == TUBE_SCROLL_OFF))
        {
            if (m_tempBrightness > 0) --m_tempBrightness;
        }
        else if (m_tempBrightness != m_brightness)
        {
            m_tempBrightness += (m_tempBrightness < m_brightness ? +1 : -1);
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
        m_state = TUBE_SCROLL_ON;
    }
    else
    {
        scrollForward();
    }
}

void  NixieTube::scrollOff(void)
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
    m_tempBrightness = tube.m_tempBrightness;
    /* Copy state variables */
    m_state = tube.m_state;
    m_next = tube.m_next;
    /* Copy flags from the source tube */
    m_flags = tube.m_flags;
    m_delayedScroll = tube.m_delayedScroll;

    /* We do not copy timer related variables */
    /* Each tube object must calculate them itself.
     * Otherwise, it breaks tube effects. */
}



