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

#include <Wire.h>
#include "nixie_audio_i2c.h"

enum
{
    CMD_NONE     = 0x0,
    CMD_START    = 0x01,
    CMD_STOP     = 0x02,
    CMD_START_ACTION = 0x03,
    CMD_START_SOUND = 0x04,
    CMD_INDOOR_LIGHT = 0x05,
    CMD_OUTDOOR_LIGHT = 0x06,
};

void NixieAudioI2c::init()
{
    if (isPresent())
    {
        m_valid = true;
    }
    else
    {
        m_valid = false;
    }
}


void NixieAudioI2c::play(uint8_t number)
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x01);
    Wire.write(number);
    Wire.write(CMD_START);
    Wire.endTransmission();
}

void NixieAudioI2c::sound(uint8_t number)
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x01);
    Wire.write(number);
    Wire.write(CMD_START_SOUND);
    Wire.endTransmission();
}


void NixieAudioI2c::play(uint8_t number, uint8_t duration)
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x01);
    Wire.write(number);
    Wire.write(CMD_START);
    Wire.endTransmission();
}

void NixieAudioI2c::action(uint8_t number)
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x01);
    Wire.write(number);
    Wire.write(CMD_START_ACTION);
    Wire.endTransmission();
}


bool NixieAudioI2c::isPlaying()
{
    if (!m_valid) return false;
    uint8_t status = 0;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR_SND, 1);
    if (Wire.available())
    {
        status = Wire.read();
    }
    return status == CMD_START;
}


uint8_t NixieAudioI2c::getCount()
{
    uint8_t count = 0;
    if (!m_valid) return count;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x03);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR_SND, 1);
    if (Wire.available())
    {
        count = Wire.read();
    }
    return count;
}

uint8_t NixieAudioI2c::getSoundCount()
{
    uint8_t count = 0;
    if (!m_valid) return count;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x04);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR_SND, 1);
    if (Wire.available())
    {
        count = Wire.read();
    }
    return count;
}


void NixieAudioI2c::stop()
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x02);
    Wire.write(CMD_STOP);
    Wire.endTransmission();
}

bool NixieAudioI2c::isPresent()
{
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x02);
    int error = Wire.endTransmission();
    if (error == 0)
    {
        return true;
    }
    return false;
}

void NixieAudioI2c::inDoorLight(uint8_t level)
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x01);
    Wire.write(level);
    Wire.write(CMD_INDOOR_LIGHT);
    Wire.endTransmission();
}

void NixieAudioI2c::outDoorLight(uint8_t level)
{
    if (!m_valid) return;
    Wire.beginTransmission(I2C_ADDR_SND);
    Wire.write(0x01);
    Wire.write(level);
    Wire.write(CMD_OUTDOOR_LIGHT);
    Wire.endTransmission();
}

