/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of Nixie Clock Library.

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

#ifndef _NIXIE_AUDIO_I2C_H_
#define _NIXIE_AUDIO_I2C_H_

#include "nixie_types.h"


/**
 * NixieAudioI2c is the I2c client to I2C sound processor
 * based on Attiny85 controller, see other library.
 * Works through standard Wire library
 */
class NixieAudioI2c
{
public:
    void init();

    NixieAudioI2c() { m_valid = false; };

    /**
     * Starts playing melody with specified number on remote sound processor.
     * The melody is played until stopped or until the end.
     */
    void    play(uint8_t number);

    /**
     * Starts playing melody with specified number on remote sound processor
     * The melody is played until stopped or until duration is off.
     */
    void    play(uint8_t number, uint8_t duration);

    /**
     * Starts playing sound on remote sound processor.
     * It is assumed that sounds and melodies are located in different memory banks.
     */
    void    sound(uint8_t number);

    /**
     * Plays action sound on remote sound processor.
     * Action sounds can be played in parallel.
     */
    void    action(uint8_t number);

    /**
     * Stops playing the melody
     */
    void    stop();

    /**
     * Returns true is remote sound processor is playing some melody
     */
    bool    isPlaying();

    /**
     * Returns true if system detected remote sound processor
     */
    bool    isPresent();

    /** 
     *  Returns number of melodies stored in memory of remote sound processor
     */
    uint8_t getCount();

    /**
     * Returns the number of sounds stored in memory of remote sound processor
     */
    uint8_t getSoundCount();

    void inDoorLight(uint8_t level);

    void outDoorLight(uint8_t level);
    
private:
    static const int I2C_ADDR_SND  = 0x04;
    bool    m_valid;
};

#endif /* _NIXIE_AUDIO_I2C_H_ */

