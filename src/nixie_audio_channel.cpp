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

#include <nixie_audio_channel.h>
#include "nixieos.h"

////////////////////////////////////////////////////////////////////////////////////////////

static inline long getNoteDuration(const uint8_t *ptr, EMelodyType type)
{
    switch (type)
    {
        case MELODY_TYPE_PROGMEM_SAMPLING:
            return 1000 * (long)pgm_read_word_near(ptr+2);
        case MELODY_TYPE_PROGMEM_TEMPO:
            return 1000000 / pgm_read_byte_near(ptr+2);
        case MELODY_TYPE_SAMPLING_NOTE:
            return 1000 * (long)( *((uint16_t *)(ptr+2)) );
        case MELODY_TYPE_TEMPO_NOTE:
            return 1000000 / (*(ptr+2));
        default:
            break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

static inline int16_t getNoteFreq(const uint8_t *ptr, EMelodyType type)
{
    switch (type)
    {
        case MELODY_TYPE_PROGMEM_SAMPLING:
        case MELODY_TYPE_PROGMEM_TEMPO:
            return pgm_read_word_near(ptr);
        case MELODY_TYPE_SAMPLING_NOTE:
        case MELODY_TYPE_TEMPO_NOTE:
            return *((int16_t *)ptr);
        default:
            break;
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////

uint8_t NixieAudioChannel::update(unsigned long ts)
{
    if (m_notes == nullptr)
    {
        m_ts = ts;
        return 0xFF;
    }
    bool updated = false;
    while (ts - m_ts >= m_halfPeriod)
    {
        m_ts += m_halfPeriod;
        m_duration -= (long)m_halfPeriod;
        if (m_totalDuration != 0)
        {
            m_totalDuration -= m_halfPeriod;
        }
        updated = true;
    }
    if (updated)
    {
        if (m_totalDuration < 0)
        {
            m_notes = nullptr;
            if (m_pin != 0xFF) nixieDigitalWrite(m_pin, LOW);
            return 0xFF;
        }
        switch (m_freq)
        {
        case NOTE_SILENT:
        case NOTE_PAUSE:
            break;
        default:
            m_pinState = !m_pinState;
            break;
        }
        if (m_pin != 0xFF) nixieDigitalWrite(m_pin, m_pinState);
        if (m_duration <= 0)
        {
            playNext();
        }
    }
    return m_pinState;
}

////////////////////////////////////////////////////////////////////////////////////////////

void NixieAudioChannel::playNote()
{
    m_freq = getNoteFreq(m_notes, m_pmode);
    switch (m_freq)
    {
    case NOTE_STOP:
        m_duration += 1000000;
        m_halfPeriod = 10000;
        m_notes = nullptr;
        digitalWrite(m_pin, LOW);
        break;
    case NOTE_SILENT:
        m_halfPeriod = 10000;
        m_duration += getNoteDuration(m_notes, m_pmode);
        break;
    default:
        m_halfPeriod = (1000000 >> 1) / m_freq;
        m_duration += getNoteDuration(m_notes, m_pmode);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

void NixieAudioChannel::playNext()
{
    if (m_freq == NOTE_PAUSE)
    {
        switch (m_pmode)
        {
            case MELODY_TYPE_PROGMEM_SAMPLING:
                m_notes+=4;
                break;
            case MELODY_TYPE_PROGMEM_TEMPO:
                m_notes+=3;
                break;
            case MELODY_TYPE_SAMPLING_NOTE:
            case MELODY_TYPE_TEMPO_NOTE:
                m_notes = nullptr;
                digitalWrite(m_pin, LOW);
                return;
            default:
                break;
        }
    }
    else
    {
        m_freq = NOTE_PAUSE;
        if (m_pause < 0)
            m_halfPeriod = getNoteDuration(m_notes, m_pmode)*(-m_pause) >> 5;
        else if (m_pause > 0)
            m_halfPeriod = (long)m_pause * 1000;
        m_duration += m_halfPeriod;
        return;
    }
    playNote();
}

////////////////////////////////////////////////////////////////////////////////////////////

void   NixieAudioChannel::playMusic(const NixieMelody &melody, long totalDuration = 0)
{
    m_notes = (char*)melody.notes;
    m_totalDuration = totalDuration;
    m_duration = 0;
    m_pmode = melody.type;
    m_pause = melody.pause;
    playNote();
}

////////////////////////////////////////////////////////////////////////////////////////////

void   NixieAudioChannel::stopMusic()
{
    digitalWrite(m_pin, LOW);
    m_notes = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////

void NixieAudioChannel::init()
{
    pinMode(m_pin, OUTPUT);
}

////////////////////////////////////////////////////////////////////////////////////////////

void   NixieAudioChannel::playNote(const SNixieTempoNote &note, bool wait)
{
    m_notes = (char*)&note;
    m_totalDuration = 0;
    m_duration = 0;
    m_pmode = MELODY_TYPE_TEMPO_NOTE;
    m_pause = 0;
    playNote();
    if ( wait )
    {
        do
        {
            update( micros() );
        } while (isPlaying());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

void   NixieAudioChannel::playNote(const SNixieSamplingNote &note, bool wait)
{
    m_notes = (char*)&note;
    m_totalDuration = 0;
    m_duration = 0;
    m_pmode = MELODY_TYPE_SAMPLING_NOTE;
    m_pause = 0;
    playNote();
    if ( wait )
    {
        do
        {
            update( micros() );
        } while (isPlaying());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

void   NixieAudioChannel::playNote(int16_t freq, long totalDuration, bool wait)
{
    SNixieSamplingNote note;
    note.freq = freq;
    note.duration = totalDuration >> 10;
    playNote(note, wait);
}

////////////////////////////////////////////////////////////////////////////////////////////


