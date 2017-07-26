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

#include "nixie_display.h"
#include "nixieos.h"

NixieDisplay::NixieDisplay(NixieDriver   & driver,
                           const uint8_t   tubePins[],
                           const uint8_t * mapTable,
                           uint8_t         maxTubes)
{
    m_maxTubes = maxTubes;
    m_brightness = NIXIE_MAX_BRIGHTNESS;
    for (uint8_t n=0; n<maxTubes; n++)
    {
        m_tubes[n].setMap(mapTable);
        m_tubes[n].setDriver(driver);
        m_tubes[n].setAnodPin(tubePins[n]);
    }
}


void NixieDisplay::init(void)
{
    for(uint8_t i=0; i<m_maxTubes; i++)
    {
        m_tubes[i].init();
    }
}


void NixieDisplay::render()
{
    bool nextTube = false;
    /* Turn off tube if we use dynamic display mode (single *
     * K155ID1 for several nixie tubes), and pwm interval   *
     * is passed. */
    if (m_tubes[m_tube].update())
    {
        nextTube = true;
    }
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
    if ( nextTube )
    {
        /* Synchronize all tubes */
        for (uint8_t i=0; i<m_maxTubes; i++)
        {
            m_tubes[i].update();
        }
        /* Select next tube */
        m_tube++; if (m_tube >= m_maxTubes) m_tube = 0;
        if ( m_powerOn )
        {
            m_tubes[m_tube].anodOn();
        }
    }
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


void NixieDisplay::__repairState(void)
{
}

void NixieDisplay::__moveLeft(void)
{
    if (m_startPos > 0)
    {
        if (g_nixieMs - m_stateTs >= SLIDE_INTERVAL)
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
        if (g_nixieMs - m_stateTs >= SLIDE_INTERVAL)
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
        if (g_nixieMs - m_stateTs >= SLIDE_INTERVAL * 3)
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
    m_state = STATE_SWAP_SMOOTH;
    m_startPos = m_maxTubes;
    m_stateTs = g_nixieMs;
    if (newBrightness != 0xFF)
    {
        m_brightness = newBrightness;
    }
    for (uint8_t i=0; i<m_maxTubes; i++)
    {
        m_tubes[i].setSmoothBrightness(0);
        m_shadowTubes[i].setBrightness(0);
        m_shadowTubes[i].setSmoothBrightness(m_brightness);
    }
    smoothOn();
};


void NixieDisplay::__swapSmooth(void)
{
    if (m_startPos == m_maxTubes)
    {
        if (g_nixieMs - m_stateTs >= 2)
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
    for(byte i=0; i<m_maxTubes; i++)
    {
        getByIndex(i).setBrightness( m_brightness );
        getByIndex(i).on();
    }
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
    m_state = STATE_MOVE_LEFT;
    m_startPos = positions;
    m_stateTs = g_nixieMs;
    on();
}

    /**
     * Scrolls all digits away to the right.
     */
void NixieDisplay::moveRight(int8_t positions)
{
    m_state = STATE_MOVE_RIGHT;
    m_startPos = -positions;
    m_stateTs = g_nixieMs;
    on();
}

void NixieDisplay::scrollForward(ENixieTubeState tubeState)
{
    for (uint8_t i=0; i<m_maxTubes; i++)
    {
        getByIndex(i).scrollForward(tubeState);
    }
}

void NixieDisplay::scroll321( )
{
    m_state = STATE_SCROLL321;
    m_startPos = - m_maxTubes;
    m_stateTs = g_nixieMs;
    __resetShadow();
    m_tubes[m_maxTubes - 1].scrollForward();
    on();
}

void NixieDisplay::scrollOn()
{
    m_powerOn = true;
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


void NixieDisplay::powerOff()
{
    m_powerOn = false;
    m_tubes[m_tube].anodOff();
}


void NixieDisplay::powerOn ()
{
    m_powerOn = true;
    NixieOs::refresh();
    for (uint8_t i=0; i<m_maxTubes; i++)
    {
        m_tubes[i].update();
    }
    m_tubes[m_tube].anodOn();
}
