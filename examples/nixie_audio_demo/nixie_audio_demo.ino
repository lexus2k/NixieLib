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

#include <nixie_library.h>
#include <nixie_audio_channel.h>
#include <nixie_melodies.h>


NixieAudioChannel   s_audio(3);


void setup()
{
    s_audio.init();
}

void loop()
{
    NixieLibrary::update();
    s_audio.update(micros());
    if (!s_audio.isPlaying( ))
    {
        s_audio.playMusic(melodyMario2);
    }
}
