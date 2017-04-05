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

/**
 This file contains NixieTube class, which describes logic for
 single NixieTube.

 @file
 @brief Nixie Library - tubes.
*/

#ifndef _NIXIE_TUBE_H_
#define _NIXIE_TUBE_H_

#include "nixie_types.h"
#include "nixie_driver.h"

/*********************************************************************
 *                      TUBE MODULE
 *
 *     NixieTube class represents single tube of Nixie Display
 *     This class doesn't control physical tube directly, but it
 *     describes the possible behavior of the tube.
 *********************************************************************/

/** How much bits to use for display brightness */
#define NIXIE_BRIGHTNESS_BITS   5

/** How quickly each digit switches to another when numerating digits in milliseconds */
#define SCROLL_INTERVAL   40

/** The blinking internal of the tube in milliseconds. Must be power of 2. */
#define BLINK_INTERVAL    ( 1<<10 )

/**
 *  Maximum brightness, supported by the NixieTube class.
 *  At present, it is range [0..32]
 */
#define NIXIE_MAX_BRIGHTNESS  (1<<NIXIE_BRIGHTNESS_BITS)

/** Interval in milliseconds for updating Nixie Tube brightness */
#define DIMMING_INTERVAL  (BLINK_INTERVAL >> NIXIE_BRIGHTNESS_BITS)

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
#define CHANGE_INTERVAL   2000 // ~ 83Hz for each tube
//#define CHANGE_INTERVAL   1600 // ~ 104Hz for each tube
//#define CHANGE_INTERVAL   800 // ~ 160Hz for each tube
#define DISPLAY_BRIGHTNESS_RANGE  (uint32_t)( NIXIE_MAX_BRIGHTNESS - 2 )

#define MIN_DESIRED_BRIGHTNESS  ( 3 * NIXIE_MAX_BRIGHTNESS / 32 )
#define MIN_NIXIE_IMPULSE_SPEC  ( 70 )  // 70us per spec
#if CHANGE_INTERVAL * MIN_DESIRED_BRIGHTNESS / NIXIE_MAX_BRIGHTNESS < MIN_NIXIE_IMPULSE_SPEC
   #error "Minimum nixie impulse should be at least 70us. Please increase change interval"
#endif

#define TUBE_FLAG_RIGHT_DOT 0x01
#define TUBE_FLAG_LEFT_DOT  0x02

// Do not use lower bit for invisible states
// Lower bit=1 means that tube is burning
typedef enum
{
    TUBE_OFF              = 0x00,       //      invisible
    TUBE_NORMAL           = 0x01,       //      visible
    TUBE_SCROLL_WRAP      = 0x03,       //      visible
    TUBE_SCROLL_FORWARD   = 0x05,       //      visible
    TUBE_SCROLL_BACK      = 0x07,       //      visible
    TUBE_SMOOTH_OFF       = 0x09,       //      visible
    TUBE_BLINK            = 0x0B,       //      visible
    TUBE_BLINK_ON         = TUBE_BLINK, // 0x0B visible
    TUBE_BLINK_OFF        = 0x0C,       //      invisible
    TUBE_SCROLL_OFF       = 0x0D,       //      visible
    TUBE_SCROLL_ON        = 0x0E,       //      invisible
} ENixieTubeState;


/**
 * This class represents logic entity related to nixie tube.
 * It doesn't change controller pins directly, but provides a lot of
 * service functions, related to physical tube effects.
 * Remember that NixieTube class supports only 10 active pins, corresponding
 * to numeric digits: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, or special symbols.
 * If you need to control additional pins like right comma, left comma,
 * use setFlags(), clearFlags() and flags() methods.
 * The purpose of mapping: If you use different type of tubes having common
 * cathods, the same cathods switch on different digits on different type of
 * tubes.
 */
class NixieTube
{
    friend class NixieDisplay;
private:
    enum
    {
        PIN_STATE_POWERED  = 0x01,
        PIN_STATE_PWM_HIGH = 0x02,
    } EPinState;
public:

    /**
     * Creates object related to one Nixie Tube. Accepts mapping table,
     * which must contain 10 elements: maps for digits to output pin of
     * K155ID1 driver or other drivers.
     *
     * @param driver - Pointer to NixieDriver class
     * @param mapTable - Pointer to map table for the used NixieDriver.
     *                   map table defines, how digits to be displayed are
     *                   mapped to logic digits in schematics.
     */
    inline NixieTube(NixieDriver *driver, const uint8_t *mapTable)
    {
        m_map = mapTable;
        m_driver = driver;
        m_pin = 0xFF;       // no-pin to control bulb Anod
        m_pinState = PIN_STATE_POWERED | PIN_STATE_PWM_HIGH;
    };

    /**
     * Creates object related to one Nixie Tube. Accepts mapping table,
     * which must contain 10 elements: maps for digits to output pin of
     * K155ID1 driver or other drivers.
     *
     * @param pin - pin to control Tube Anod.
     * @param driver - Pointer to NixieDriver class
     * @param mapTable - Pointer to map table for the used NixieDriver.
     *                   map table defines, how digits to be displayed are
     *                   mapped to logic digits in schematics.
     */
    inline NixieTube(uint8_t pin, NixieDriver *driver, const uint8_t *mapTable)
    {
        m_map = mapTable;
        m_driver = driver;
        m_pin = pin;        // pin controlling Anod power
        m_pinState = PIN_STATE_POWERED | PIN_STATE_PWM_HIGH;
    };


    /**
     * This method must be called in the cycle. It updates the state of the tube.
     * @return returns true when state of the tube is recalculated.
     *         It happens every CHANGE_INTERVAL microseconds 
     */
    bool               update();

    /**
     * Initializes NixieTube state.
     */
    void               init();
    
    /** 
     *  Returns actual pin number to activate to displayed specific symbol on Nixie bulb.
     *  @warning Make sure that isBurning() method returns true!! Otherwise, the program will crash
     */
    inline uint8_t     getActivePin   ()                                   { return m_map[m_digit]; };
    
    /** Returns true if bulb digit should be visible. Doesn't correlate with PWM state */
    inline bool        isBurning      ()                                   { return (m_digit != 0xFF) && isEnabled(); };
    
    /** Returns true if the bulb is not switched off */
    inline bool        isEnabled      (void)                               { return (m_state & 0x01) != 0; };
    
    /** 
     *  Returns value, programmed to the bulb to display
     *  This value differs from the displayed digit, because displayed digit depends on
     *  the bulb state
    */
    inline uint8_t     value          (void)                               { return m_value; };

    /** Returns flags by specified mask. If to pass 0xFF, all flags will be returned
     *  @param[in] mask - bit mask for the flags to read
     *  @return flags value, covered by bit mask.
     */
    inline bool        flags          (uint8_t mask)                       { return (m_flags & mask) != 0; };

    /** Sets flags by specified mask.
     *  @param[in] mask - mask for the flags to turn bits on
     */
    inline void        setFlags       (uint8_t mask)                       { m_flags |= mask; };

    /** Clears flags by specified mask.
     *  @param[in] mask - mask for the flags to turn bits off
     */
    inline void        clearFlags     (uint8_t mask)                       { m_flags &= (~mask); };

    /** Resets current brightness to set brightness and value to 0. */
    inline void        clear          ()                                   { m_value = 0; m_flags = 0;
                                                                             setActiveBrightness(m_brightness); };
    
    /**
     * Turns off the nixie tube. Turning off means that isBurning() and isEnabled() returns false.
     */
    inline void        off            (void)                               { m_tempBrightness = 0; m_state = TUBE_OFF; };

    /**
     * Turns on the bulb with wrap effect
     */
    inline void        wrapOn         (void)                               { scrollForward(); };
    
    /**
     * Turns on the nixie tube. Switches the tube to the TUBE_NORMAL state.
     */
    inline void        on             (void)                               { m_state = TUBE_NORMAL; };
    
    /** Turns on the tube by smoothly increasing tube brightness to the set brightness value */
    inline void        smoothOn       (void)                               { m_state = TUBE_NORMAL; };

    /** Turns off the tube by smoothly decreasing tube brightness to the zero and switching to TUBE_OFF state */
    inline void        smoothOff      (void)                               { m_state = TUBE_SMOOTH_OFF; };

    /** Initiates wrapping digits effect and finally switching to specified state.
     *  Wrapping effect lasts for about 0.5 seconds.
     */
    inline void        scrollForward  (ENixieTubeState next = TUBE_NORMAL) { m_state = TUBE_SCROLL_FORWARD; m_delayedScroll = 10;
                                                                             m_next = next; m_digit = m_value; };
    /** Initiates wrapping digits effect and finally switching to specified state.
     *  Wrapping effect lasts for about 0.5 seconds. The difference from scrollForward() is that
     *  digits are being changed in reverse order: 5, 4, 3, 2, 1, 0, 9, etc.
     */
    inline void        scrollBack     (ENixieTubeState next = TUBE_NORMAL) { m_state = TUBE_SCROLL_BACK; m_delayedScroll = 10;
                                                                             m_next = next; m_digit = m_value; };

    /** Starts wrapping digits effect finally switching tube to TUBE_OFF state */
    void               scrollOff      (void);

    /** Starts wrapping digits effect finally switching tube to TUBE_OFF state
     *  @param[in] msDelay - additional delay in milliseconds before turning tube off
     */
    void               scrollOff      (uint16_t msDelay);

    /** Starts wrapping digits effect smoothly switching tube on. */
    void               scrollOn       (void);

    /** Starts wrapping digits effect smoothly switching tube on.
     *  @param[in] msDelay - additional delay in milliseconds before turning tube on
     */
    void               scrollOn       (uint16_t msDelay);

    /** Starts blinking effect. The effect needs to be stopped by noBlink() function. */
    inline void        blink          (void)                               { m_state = TUBE_BLINK; };

    /** Stops blinking effect. */
    inline void        noBlink        (void)                               { m_state = TUBE_NORMAL; };

    /** Changes displayed digit */
    inline void operator =            (const uint8_t val)                  { m_value = val & 0x0F; };

    /** Copies state from the source tube. */
    void        operator =            (const NixieTube &tube);

    /** Compares value asigned to NixieTube with value provided */
    bool        operator >=           (const uint8_t val)                  { return m_value >= val; };

    /** Compares value asigned to NixieTube with value provided */
    bool        operator <=           (const uint8_t val)                  { return m_value <= val; };

    /** Compares value asigned to NixieTube with value provided */
    bool        operator >            (const uint8_t val)                  { return m_value > val; };

    /** Compares value asigned to NixieTube with value provided */
    bool        operator <            (const uint8_t val)                  { return m_value < val; };

    /** Increments value assigned to NixieTube */
    uint8_t     operator +            (const int8_t val)                   { return m_value + val; };

    /** Decrements value assigned to NixieTube */
    uint8_t     operator -            (const int8_t val)                   { return m_value - val; };

    /** Returns set brightness for the tube in range [0..32] */
    inline uint8_t     getBrightness  ()                                   { return m_brightness; };

    /** Returns active brightness for the tube, which depends on the tube state. [0..32] */
    inline uint8_t     getActiveBrightness()                               { return m_tempBrightness; };

    /** Sets tube brightness immediately in the range [0..32] */
    inline void        setBrightness     (uint8_t level)                   { m_brightness = level; setActiveBrightness(level); };

    /** Sets target brightness for the tube in the range [0..32]. Active brightness will slowly move to the target brightness */
    inline void        setSmoothBrightness(uint8_t level)                  { m_brightness = level; };

    /** Sets map for the tube. This map defines pin number of the nixie driver microchip, which should be
     *  enabled for specific digit.
     */
    inline void        setMap(const uint8_t *mapTable)                     { m_map = mapTable; };

    /** Sets NixieDriver for the tube */
    inline void        setDriver(NixieDriver & driver)                     { m_driver = &driver; };

    /** Sets Anod pin for the tube */
    inline void        setAnodPin(uint8_t pin)                             { m_pin = pin; };

    /** Works only for the tubes with specified Anod pin-control */
    void               anodOff();

    /** Works only for the tubes with specified Anod pin-control */
    void               anodOn();
    
private:
    /**
     * Creates object related to one Nixie Tube. Warning!!! 
     * mapping table must be set via setMap() method before working with this object
     */
    inline NixieTube()
    {
        m_map = nullptr;
        m_driver = nullptr;
        m_pin = 0xFF;
        m_pinState = 0x00;
    }

    
    const uint8_t    * m_map;
    NixieDriver *      m_driver;

    /** Defines uC pin, which controls Anod power */
    const uint8_t    * m_pin;      

    /** Defines whever Power can be applied to Anod */
    uint8_t            m_pinState    = 0; 

    /** Current digit set */
    uint8_t            m_value      = 0;

    /** Actually displayed digit. It depends on used Nixie tube effect */
    uint8_t            m_digit      = 0;

    /** Set brightness */
    uint8_t            m_brightness;

    /** Active brightness */
    uint8_t            m_tempBrightness;

    /** Current timestamp milliseconds */
    uint16_t           m_mts        = 0;

    /** Current timestamp microseconds */
    uint16_t           m_uts        = 0;

    /** Impulse width when Anod is On. Has no meaning for tubes without conofigured Anod */
    uint16_t           m_impulseWidth = 0;

    /** Last time the tube brightness was modified */
    uint16_t           m_dimmingTs  = 0;

    /** Flags set for the tube */
    uint8_t            m_flags      = 0;

    /** Current tube state */
    ENixieTubeState    m_state      = TUBE_NORMAL;

    /** Next tube state, when effect is over */
    ENixieTubeState    m_next       = TUBE_NORMAL;

    /** This parameter used different ways */
    uint8_t            m_delayedScroll    = 0;

    /** Updates active tube brightness */
    void               updateDimming  ();

    /** Sets active brightness */
    void               setActiveBrightness( uint8_t brightness );

    void               updatePinState();
};



#endif


