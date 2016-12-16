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

#ifndef _NIXIE_DISPLAY_H_
#define _NIXIE_DISPLAY_H_

#include "nixie_tube.h"
#include "nixie_driver.h"
#include "nixie_booster.h"
/*********************************************************************
 *                      DISPLAY MODULE
 *
 *     NixieDisplay represents all physical tubes and
 *     provides special effects.
 *********************************************************************/

/*
 * IN-12/IN-14 Datasheet.
 * Impulse mode: 70us / refresh frequency 1-1.8kHz
 */

/* Burning time for each digit.
 * When 1 digit is ON, other are OFF. This means that during full cycle 
 * bulb is OFF for CHANGE_INTERVAL*5.
 * IMPULSE_WIDTH should be greater than 70us (according to IN-14 spec) and working
 * frequency should be below 1-1.8kHz, otherwise the gas in the the tube will be visible.
 * On other way, Atmega328 supports only 4us accuracy.
 * Nixie Display module uses 32 levels of brightness, thus to have 25% accuracy on low brightness
 * minimum change interval should be 100/25 * 4 * 64 = 512.
 */
//#define CHANGE_INTERVAL   1600 // ~ 104Hz for each bulb
#define CHANGE_INTERVAL   800 // ~ 160Hz for each bulb
#define DISPLAY_BRIGHTNESS_RANGE  (uint32_t)( NIXIE_MAX_BRIGHTNESS - 2 )

/* Slide interval points how fast digits are moved away from the screen */
#define SLIDE_INTERVAL    50000

#define REPAIR_TIMEOUT    1000000

#define NIXIE_MAX_BULBS         ( 6 )
#define MIN_DESIRED_BRIGHTNESS  ( 3 * NIXIE_MAX_BRIGHTNESS / 32 )
#define MIN_NIXIE_IMPULSE_SPEC  ( 70 )  // 70us per spec
#if CHANGE_INTERVAL * MIN_DESIRED_BRIGHTNESS / NIXIE_MAX_BRIGHTNESS < MIN_NIXIE_IMPULSE_SPEC
   #error "Minimum nixie impulse should be at least 70us. Please increase change interval"
#endif

/**
 * Nixie Display encapsulates several tubes
 * 
 */
class NixieDisplay
{
    typedef enum
    {
        STATE_NORMAL,
        STATE_MOVE_LEFT,
        STATE_MOVE_RIGHT,
        STATE_SCROLL321,
        STATE_SWAP_SMOOTH,
        STATE_DISPLAY_OFF,
        STATE_REPAIR_TUBES,
    } ENixieDisplayState;
    
public:
    /**
     * Creates object representing display consisting of 1 - 6 Nixie Tubes
     * @param[in] driver - K155ID1 driver to control all Nixie Tubes (yes, it is the only one - cost reduction)
     * @param[in] tubePins - array, containing pin numbers corresponding to controlled tube's Anods
     * @param[in] mapTable - default pin mapping for all NixieTubes (refer to NixieTube class)
     * @param[in] maxTubes - 1 to 6
     */
    inline NixieDisplay(NixieDriver    &driver,
                        const uint8_t   tubePins[],
                        const uint8_t  *mapTable,
                        uint8_t         maxTubes)
    {
        m_driver   = &driver;
        m_tubePins = tubePins;
        m_maxTubes = maxTubes;
        for (uint8_t n=0; n<maxTubes; n++)
        {
            m_tubes[n].setMap(mapTable);
        }
    };

    /**
     * This method must be called in setup function
     */
    void init(void);
    
    /**
     * This method must be called in loop function
     * @param ts - long time in microseconds
     */
    void render(unsigned long ts);
    
    /**
     * Turns display off (immediately)
     */
    inline void turnOff()  { m_state = STATE_DISPLAY_OFF; }

    /**
     * Turns display on (immediately)
     */
    inline void turnOn ()  { m_state = STATE_NORMAL; };


    //======================== Effects ==================================

    /** Makes all Nixie tubes to blink */
    void blink();

    /** Switches all nixie tubes to normal mode */
    void noBlink();
 
    /** Smoothly turns on nixie tubes */
    void smoothOn();

    /** Turns on nixie tubes */
    void on();
    
    /** Turns off nixie tubes. This means that module will never enable
     *  Anod on the tubes.
     */
    void off();
    
    void smoothOff();
    
    /**
     * Scrolls all digits away to the left.
     */
    void moveLeft(int8_t positions = 12);
    
    /**
     * Scrolls all digits away to the right.
     */
    void moveRight(int8_t positions = 12);

    void scrollOn();
    
    void scrollOff();

    void randomEffect();
    
    /**
     * Smoothly dimms all tubes with one content and then dimms on the tubes with another content
     * @param newBrightness - new value for the brightness (0xFF if the brightness should not be changed to new value)
     */
    void swapSmooth(byte newBrightness = 0xFF);

    /**
     * Starts to scroll from last digit till the first one.
     * Unlike scrollForward() method it scrolls digits one by one. Not all together
     */
    void scroll321( );

    /**
     * Starts to scrolls all digits forward 
     * until they reach new values
     */
    void scrollForward(ENixieTubeState tubeState = TUBE_NORMAL);
    
    /**
     *  Wraps all digits on the display once.
     */
    void repair();

    /**
     * Clears all flags of Nixie Tubes
     */
    void clearFlags();

    /**
     * Brightness in the range 0 - 64
     */
    void setBrightness(byte level);

    /**
     * Brightness in the range 0 - 64
     */
    uint8_t getBrightness();

    /**
     * Returns NixieTube object by its index
     * startPos=0   startPos=2   startPos=4
     * 0123 XXXX    XX01 XX23    XXXX 0123
     * 
     */
    NixieTube &operator [](int index)
    {
        return getByIndex(index);
    };

    inline NixieTube& getByIndex(int index)
    {
        int8_t idx = index + m_startPos;
        if ( (idx <0) || (idx >= m_maxTubes) )
            return m_shadowTubes[index];
        if ( m_state == STATE_SCROLL321)
            return m_tubes[index];
        return m_tubes[idx];
    };


    uint16_t getImpulseWidth() { return m_impulseWidth; };
    
private:
    inline void __tubeOn(byte pos)  { if (m_tubePins != nullptr) nixiePinHigh(m_tubePins[pos]); };
    inline void __tubeOff(byte pos) { if (m_tubePins != nullptr) nixiePinLow(m_tubePins[pos]); };
    void        __swapSmooth(void);
    void        __controlImpulseOff(void);
    void        __controlImpulseOn(void);
    void        __resetShadow(void);
    /* States */
    void        __normalState(void);
    void        __repairState(void);
    void        __moveLeft(void);
    void        __moveRight(void);
    void        __scroll321(void);

    /* Number of the tubes supported by NixieDisplay */
    int8_t                     m_maxTubes;
    /* K155ID1 driver chip to control the digits on the tube */
    NixieDriver               *m_driver;
    /* These tubes are visible on the Tube Display */
    NixieTube                  m_tubes[6];
    /* These tubes are hidden on the Tube Display */
    NixieTube                  m_shadowTubes[6];
    const byte                 *m_tubePins;
    int8_t                     m_startPos    = 0;
    byte                       m_tube        = 0;
    ENixieDisplayState         m_state       = STATE_NORMAL;
    uint16_t                   m_tubeTs      = 0;
    unsigned long              m_activeTs    = 0;
    bool                       m_tubeBurning = false;
    byte                       m_brightness;

    /* State-specific. Each state uses fields below for their own needs */
    unsigned long              m_stateTs      = 0;
    uint16_t                   m_impulseWidth = CHANGE_INTERVAL;
    uint8_t                    m_lastEffect   = 0;

    uint16_t                   m_impulses[NIXIE_MAX_BRIGHTNESS + 1];         
};

#endif

