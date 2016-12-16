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

#include "nixie_display.h"

static const uint32_t s_newInterval = ( (uint32_t)CHANGE_INTERVAL * DISPLAY_BRIGHTNESS_RANGE ) / (uint32_t)NIXIE_MAX_BRIGHTNESS;
static const uint32_t s_minChangeInterval = (uint32_t)CHANGE_INTERVAL - s_newInterval;

void NixieDisplay::init(void)
{
    if (m_tubePins != nullptr)
    {
        for(uint8_t i=0; i<m_maxTubes; i++)
            pinMode(m_tubePins[i], OUTPUT);
    }
    // Calculate boost table
    for(uint8_t brightness = 0; brightness <= NIXIE_MAX_BRIGHTNESS; brightness++)
    {
        m_impulses[brightness] = (uint16_t)(( s_minChangeInterval + s_newInterval * (uint32_t)brightness) >> NIXIE_BRIGHTNESS_BITS );
    }
}


void NixieDisplay::render(unsigned long ts)
{
    m_activeTs = ts;
    /* Turn off tube if needed */
    __controlImpulseOff();
    switch (m_state)
    {
    case STATE_MOVE_LEFT:
        __moveLeft();
        break;
    case STATE_MOVE_RIGHT:
        __moveRight();
        break;
    case STATE_SCROLL321:
        __scroll321();
        break;
    case STATE_SWAP_SMOOTH:
        __swapSmooth();
        break;
    case STATE_NORMAL:
        break;
    case STATE_REPAIR_TUBES:
        break;
    default:
        break;
    }
    __normalState();
    /* Turn on tube if needed */
}


void NixieDisplay::repair()
{
    for (byte i=0; i<m_maxTubes; i++)
        m_tubes[i].scrollForward();
}


void NixieDisplay::clearFlags()
{
    for (byte i=0; i<m_maxTubes; i++)
        getByIndex(i).clearFlags(0xFF);
}


void NixieDisplay::setBrightness(byte level)
{
    m_brightness = level;
    for (byte i=0; i<m_maxTubes; i++)
    {
        m_tubes[i].setSmoothBrightness(m_brightness);
        m_shadowTubes[i].setSmoothBrightness(m_brightness);
    }
}


uint8_t NixieDisplay::getBrightness()
{
    return m_brightness;
}


void NixieDisplay::__controlImpulseOff(void)
{
    /* If nixie tube had enough impulse, just power off it */
    if ((true == m_tubeBurning) && ((uint16_t)m_activeTs - m_tubeTs >= m_impulseWidth))
    {
        __tubeOff(m_tube);
        m_tubeBurning = false;
    }
}


void NixieDisplay::__controlImpulseOn(void)
{
    if (m_tubeBurning == false)
    {
         if (m_state != STATE_DISPLAY_OFF)
         {
             __tubeOn(m_tube);
             m_tubeBurning = true;
             uint8_t brightness = m_tubes[m_tube].getActiveBrightness();
             if (brightness > NIXIE_MAX_BRIGHTNESS)
             {
                 m_impulseWidth = CHANGE_INTERVAL;
             }
             else
             {
                 m_impulseWidth = m_impulses[brightness];
             }
         }
    }
}


void NixieDisplay::__normalState(void)
{
    if ((uint16_t)m_activeTs - m_tubeTs >= CHANGE_INTERVAL)
    {
        // Consider that X/1000 ~ X/1024 to reduce calculation cost
        /* Process only visible tubes. Consider shadowTubes as not needed for update */
        uint16_t ticks = (uint16_t)(m_activeTs >> 10);
        for (byte i=0; i<m_maxTubes; i++)
            m_tubes[i].update( ticks );
        m_tubeTs = m_activeTs; //+= CHANGE_INTERVAL;
        // Select next tube
        if (++m_tube >= m_maxTubes) m_tube = 0;
        // Set digit code for new digit to display.
        if ( m_tubes[m_tube].isBurning() )
        {
            m_driver->switchPin(m_tubes[m_tube].getActivePin());
            m_driver->setExtendedPins(m_tubes[m_tube].flags(0xFF));
            // Turn ON next bulb
            __controlImpulseOn();
        }
    }
}

void NixieDisplay::__repairState(void)
{
/*    if (m_activeTs - m_stateTs >= SLIDE_INTERVAL)
    {
        m_
    }*/
}

void NixieDisplay::__moveLeft(void)
{
    if (m_startPos > 0)
    {
        if (m_activeTs - m_stateTs >= SLIDE_INTERVAL)
        {
            m_stateTs += SLIDE_INTERVAL;
            for (uint8_t i=1; i<m_maxTubes; i++)
            {
                m_tubes[i-1] = m_tubes[i]; 
            }
            if (--m_startPos < m_maxTubes)
            {
                m_tubes[m_maxTubes-1] = m_shadowTubes[m_maxTubes - m_startPos - 1];
                m_shadowTubes[m_maxTubes - m_startPos - 1].clear();
            }
            else
            {
                m_tubes[m_maxTubes - 1].off();
            }
        }
    }
    else
    {
        m_state = STATE_NORMAL;
    }
}


void NixieDisplay::__moveRight(void)
{
    if (m_startPos < 0)
    {
        if (m_activeTs - m_stateTs >= SLIDE_INTERVAL)
        {
            m_stateTs += SLIDE_INTERVAL;
            for (uint8_t i=m_maxTubes-1; i>0; i--)
            {
                m_tubes[i] = m_tubes[i-1]; 
            }
            if (++m_startPos > -m_maxTubes)
            {
                m_tubes[0] = m_shadowTubes[-m_startPos];
                m_shadowTubes[-m_startPos].clear();
            }
            else
            {
                m_tubes[0].off();
            }
        }
    }
    else
    {
        m_state = STATE_NORMAL;
    }
}


void NixieDisplay::__scroll321( )
{
    if (m_startPos < 0)
    {
        if (m_activeTs - m_stateTs >= SLIDE_INTERVAL * 3)
        {
            m_stateTs += SLIDE_INTERVAL * 3;
            m_tubes[ -m_startPos - 1 ] = m_shadowTubes[ -m_startPos - 1];
            m_startPos++;
            if (m_startPos < 0)
            {
                m_tubes[ -m_startPos - 1 ].scrollForward();
            }
        }
    }
    else
    {
        m_state = STATE_NORMAL;
    }
}


void NixieDisplay::swapSmooth(byte newBrightness)
{
    if (m_state != STATE_DISPLAY_OFF)
    {
        m_state = STATE_SWAP_SMOOTH;
        m_startPos = m_maxTubes;
        m_stateTs = m_activeTs;
        if (newBrightness != 0xFF)
        {
            m_brightness = newBrightness;
        }
        for (byte i=0; i<m_maxTubes; i++)
        {
            m_tubes[i].setSmoothBrightness(0);
            m_shadowTubes[i].setBrightness(0);
            m_shadowTubes[i].setSmoothBrightness(m_brightness);
        }
        smoothOn();
    }
};


void NixieDisplay::__swapSmooth(void)
{
    if (m_startPos == m_maxTubes)
    {
        if (m_activeTs - m_stateTs >= (DIMMING_INTERVAL << 1) * m_brightness)
        {
            for(byte i=0; i<m_maxTubes; i++) m_tubes[i] = m_shadowTubes[i];
            m_startPos = 0;
            m_state = STATE_NORMAL;
        }
    }
}

void NixieDisplay::__resetShadow(void)
{
    for (byte i=0; i< m_maxTubes; i++)
        m_shadowTubes[i].on();
}


// ================================ Effects ===========================================

void NixieDisplay::blink()
{
    for (byte i=0; i< m_maxTubes; i++)
        getByIndex(i).blink();
}


void NixieDisplay::noBlink()
{
    for (byte i=0; i< m_maxTubes; i++)
        getByIndex(i).noBlink();
}

void NixieDisplay::smoothOn()
{
    for(byte i=0; i<m_maxTubes; i++) getByIndex(i).smoothOn();
}

void NixieDisplay::on()
{
    for(byte i=0; i<m_maxTubes; i++) getByIndex(i).on();
}

void NixieDisplay::off()
{
    for(byte i=0; i<m_maxTubes; i++) getByIndex(i).off();
}

void NixieDisplay::smoothOff()
{
    for(byte i=0; i<m_maxTubes; i++) getByIndex(i).smoothOff();
}

    /**
     * Scrolls all digits away to the left.
     */
void NixieDisplay::moveLeft(int8_t positions)
{
    if (m_state != STATE_DISPLAY_OFF)
    {
        m_state = STATE_MOVE_LEFT;
        m_startPos = positions;
        m_stateTs = m_activeTs;
        on();
    }
}

    /**
     * Scrolls all digits away to the right.
     */
void NixieDisplay::moveRight(int8_t positions)
{
    if (m_state != STATE_DISPLAY_OFF)
    {
        m_state = STATE_MOVE_RIGHT;
        m_startPos = -positions;
        m_stateTs = m_activeTs;
        on();
    }
}

void NixieDisplay::scrollForward(ENixieTubeState tubeState)
{
    for (byte i=0; i<m_maxTubes; i++) getByIndex(i).scrollForward(tubeState);
}

void NixieDisplay::scroll321( )
{
    if (m_state != STATE_DISPLAY_OFF)
    {
        m_state = STATE_SCROLL321;
        m_startPos = - m_maxTubes;
        m_stateTs = m_activeTs;
        __resetShadow();
        m_tubes[m_maxTubes - 1].scrollForward();
    }
}

void NixieDisplay::scrollOn()
{
    for (byte i=0; i<m_maxTubes; i++)
        if (i<(m_maxTubes>>1))
            getByIndex(i).scrollOn( (uint16_t)((m_maxTubes>>1) - 1 - i) * (SCROLL_INTERVAL<<2));
        else
            getByIndex(i).scrollOn( (uint16_t)(i - (m_maxTubes>>1)) * (SCROLL_INTERVAL<<2 ));
}


void NixieDisplay::scrollOff()
{
    for (byte i=0; i<m_maxTubes; i++)
        if (i<(m_maxTubes>>1))
            getByIndex(i).scrollOff( (uint16_t)(i) * (SCROLL_INTERVAL<<2));
        else
            getByIndex(i).scrollOff( (uint16_t)(m_maxTubes - i - 1) * (SCROLL_INTERVAL<<2 ));
}

void NixieDisplay::randomEffect()
{
    switch (m_lastEffect)
    {
    case 0:
        scrollForward();
        break;
    case 1:
        scroll321();
        break;
    default:
        break;
    }
    if ( ++m_lastEffect >= 2)
    {
        m_lastEffect = 0;
    }
}


