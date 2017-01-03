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

#ifndef _NIXIE_AUDIO_CHANNEL_H_
#define _NIXIE_AUDIO_CHANNEL_H_

#include <nixie_types.h>
#include <nixie_audio_defs.h>

/**
 * SNixieTempoNote defines note as pair of 2 numbers:
 *    1. note frequency
 *    2. note duration in parts of the second.
 *    
 * Tempo '2' means 1/2 sec, tempo '8' means 1/8 sec.
 * This way of definition needs only 3 bytes per note
 * and is easy for translating Notes to digital representation.
 */
typedef struct
{
    int16_t    freq;
    uint8_t    tempo;
} SNixieTempoNote;

/**
 * SNixieSamplingNote defines note as pair of 2 numbers:
 *     1. Note frequency
 *     2. Note duration in milliseconds
 *     
 * This way of definition needs more bytes (4), but can be used
 * to convert PCM to Arduino music with specific sampling frequency.
 */
typedef struct
{
    int16_t    freq;
    int16_t    duration;
} SNixieSamplingNote;

/**
 * Define melody types, which are used to encode melody
 */
typedef enum
{
    MELODY_TYPE_PROGMEM_TEMPO,
    MELODY_TYPE_PROGMEM_SAMPLING,
    MELODY_TYPE_TEMPO_NOTE,
    MELODY_TYPE_SAMPLING_NOTE,
} EMelodyType;

typedef struct
{
    const uint8_t *notes;
    EMelodyType    type;
    /** pause in milliseconds between playing notes
     *  or if negative - defines pause between note in % of note duration (in 1/32 units).
     */
    int16_t        pause; 
} NixieMelody;


/** NixieAudioChannel class is intended to play defined melodies on buzzer,
 *  connected to specific atmel controller pin.
 */
class NixieAudioChannel
{
public:
     /**
      *  Creates NixieAudioChannel object.
      *  @param[in] pin - buzzer pin to control. Set it to 0xFF if
      *             you want to control buzzer pin manually.
      *  @see update()
      */
     inline NixieAudioChannel(uint8_t pin) { m_pin = pin; };

     /** Initializes and sets correct pin mode */
     void init();

     /**
      *  This method must be called in main loop cycle always.
      *  @param[in] ts - timestamp returned by micros() function.
      *  @returns LOW or HIGH, saying which state should be set Digital pin to.
      *           This value can be used to manually control the buzzer pin,
      *           if you need.
      */
     uint8_t update(unsigned long ts);

     /**
      * Starts playing specified melody.
      * @warn melody notes should be placed to flash memory, not RAM
      * 
      * @param[in] melody - melody to play
      * @param[in] totalDuration - desired duration in microseconds or 0.
      */
     void   playMusic(const NixieMelody &melody, long totalDuration = 0);

     /** Plays single tempo note, located in RAM.
      *  @param[in] note - reference to SNixieTempoNote structure, describing
      *                    the note to play
      *  @param[in] wait - if false, playNote() will return immediately, and
      *                             don't forget to call update() in loop cycle.
      *                    if true, playNote() will return, when playing is finished.
      *                    For "true" case playNote() will call update() automatically.
      *  @note call isPlaying() method to get know if playing is stopped or not.
      */
     void   playNote(const SNixieTempoNote &note, bool wait = false);

     /** Plays single sampling note, located in RAM.
      *  @param[in] note - reference to SNixieSamplingNote structure, describing
      *                    the note to play
      *  @param[in] wait - if false, playNote() will return immediately, and
      *                             don't forget to call update() in loop cycle.
      *                    if true, playNote() will return, when playing is finished.
      *                    For "true" case playNote() will call update() automatically.
      *  @note call isPlaying() method to get know if playing is stopped or not.
      */
     void   playNote(const SNixieSamplingNote &note, bool wait = false );

     /** Plays speicified frequency during specified duration in microseconds.
      *  @param[in] freq - frequency in Herz to play
      *  @param[in] totalDuration - how log to play the note in microseconds.
      *  @param[in] wait - if false, playNote() will return immediately, and
      *                             don't forget to call update() in loop cycle.
      *                    if true, playNote() will return, when playing is finished.
      *                    For "true" case playNote() will call update() automatically.
      *  @note call isPlaying() method to get know if playing is stopped or not.
      */
     void   playNote(int16_t freq, long totalDuration, bool wait = false );

     /**
      * Stops playing music or note.
      */
     void   stopMusic();

     /** Returns true if playing something */
     bool isPlaying() { return m_notes != nullptr; };

private:
     EMelodyType       m_pmode = MELODY_TYPE_PROGMEM_TEMPO;
     byte              m_pin;
     unsigned long     m_ts;             /* time in useconds */
     unsigned long     m_totalDuration = 0;

     unsigned long     m_halfPeriod;     /* time in useconds */
     long              m_duration = 0;   /* time in mseconds */
     bool              m_pinState = false;
     int               m_freq = 0;
     int               m_pause = 0;

     const char*       m_notes = nullptr;

     void  playNote();
     void  playNext();
};

#endif /* _NIXIE_AUDIO_CHANNEL_H_ */

