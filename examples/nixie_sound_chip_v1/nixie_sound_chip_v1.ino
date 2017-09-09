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
/**
 *   Attiny85 PINS
 *                       ____
 *   RESET             -|_|  |- 3V
 *   LM386M        (3) -|    |- (2) - SCL
 *   Indoor lights (4) -|    |- (1) - Outdoor lights
 *   GND               -|____|- (0) - SDA
 *
 */

#define I2C_SLAVE_ADDRESS   0x04

#include <nixie_i2c_client.h>
#include <nixie_audio_channel.h>
#include <nixie_melodies.h>


const uint8_t audioPin       =  3;
const uint8_t outdoorPin     =  1;
const uint8_t indoorPin      =  4;

const NixieMelody melodyProfiles[] = {
    melodyMario2,
    melodyMarioUnderground,
    melodyMonkeyIslandP,
};

const NixieMelody soundProfiles[] = {
    soundClock,
    doorBell,
};

const NixieMelody melodyActions[] = {
    soundClick,
    soundOk,
    soundCancel,
};


NixieAudioChannel   g_audio(audioPin);
     
unsigned long       g_mts;
unsigned long       g_uts;

 
volatile uint8_t i2c_regs[] =
{
    0x00,   // | 0x00 - status   0x00 - STOPPED, 0x01 - STARTED
    0x00,   // | 0x01 - melody index
    0x00,   // | 0x02 - command  0x00 - NONE,    0x01 - START,   0x02 STOP
    sizeof(melodyProfiles)/sizeof(NixieMelody), // | 0x03 - melodies count
    sizeof(soundProfiles)/sizeof(NixieMelody), // | 0x04 - sounds count
};

const uint8_t i2c_access[sizeof(i2c_regs)] =
{
    I2C_READ_ONLY,
    I2C_READ_WRITE,
    I2C_READ_WRITE,
    I2C_READ_ONLY,
    I2C_READ_ONLY
};


// Tracks the current register pointer position
volatile byte reg_position = 0;
#define reg_size   sizeof(i2c_regs)

#define SCOUNT_REG   i2c_regs[4]
#define COUNT_REG    i2c_regs[3]
#define COMMAND_REG  i2c_regs[2]
#define MELODY_REG   i2c_regs[1]
#define STATUS_REG   i2c_regs[0]

enum
{
    CMD_NONE          = 0x0,
    CMD_START         = 0x01,
    CMD_STOP          = 0x02,
    CMD_PLAY_ACTION   = 0x03,
    CMD_PLAY_SOUND    = 0x04,
    CMD_HOME_LIGHT    = 0x05,
    CMD_OUTDOOR_LIGHT = 0x06,
};

static bool lightTest = true;
unsigned long lightTestMs;

void setup()
{
    SimpleClientI2c::setRegisters(i2c_regs, i2c_access, reg_size);
    SimpleClientI2c::begin(I2C_SLAVE_ADDRESS);

    pinMode(outdoorPin, OUTPUT);
    digitalWrite(outdoorPin, HIGH);
    pinMode(indoorPin, OUTPUT);
    digitalWrite(indoorPin, HIGH);
    g_audio.init();
    lightTestMs = millis();
}

void loop()
{
    g_uts = micros();
    g_audio.update(g_uts);
    SimpleClientI2c::update();
    switch (COMMAND_REG)
    {
    case CMD_START:
        if (MELODY_REG < sizeof(melodyProfiles)/sizeof(NixieMelody))
        {
            g_audio.playMusic(melodyProfiles[MELODY_REG]);
            COMMAND_REG = CMD_NONE;
            STATUS_REG = CMD_START;
        }
        else
        {
            COMMAND_REG = CMD_NONE;
        }
        break;
    case CMD_PLAY_SOUND:
        if (MELODY_REG < sizeof(soundProfiles)/sizeof(NixieMelody))
        {
            g_audio.playMusic(soundProfiles[MELODY_REG]);
            COMMAND_REG = CMD_NONE;
            STATUS_REG = CMD_START;
        }
        else
        {
            COMMAND_REG = CMD_NONE;
        }
        break;
    case CMD_STOP:
        g_audio.stopMusic();
        COMMAND_REG = CMD_NONE;
        STATUS_REG = CMD_NONE;
        break;
    case CMD_PLAY_ACTION:
        if (MELODY_REG < sizeof(melodyActions)/sizeof(NixieMelody))
        {
            g_audio.playMusic(melodyActions[MELODY_REG]);
            COMMAND_REG = CMD_NONE;
        }
        else
        {
            COMMAND_REG = CMD_NONE;
        }
        break;
    case CMD_HOME_LIGHT:
        analogWrite(indoorPin, MELODY_REG);
        break;
    case CMD_OUTDOOR_LIGHT:
        analogWrite(outdoorPin, MELODY_REG);
        break;
    default:
        break;
    }
    if (!g_audio.isPlaying( ))
    {
        STATUS_REG = CMD_NONE;
    }
    if (lightTest)
    {
        if (millis()-lightTestMs > 10000)
        {
            lightTest = false;
            analogWrite(indoorPin, 0);
            analogWrite(outdoorPin, 0);
        }
    }
}
