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

#include "nixieos_events.h"

NixieEvents::NixieEvents()
{
    for(uint8_t i=0; i<MAX_NIXIE_EVENTS; i++)
    {
        m_events[i].event = 0;
    }
}

bool NixieEvents::put(uint8_t event, uint8_t param)
{
    for (uint8_t i=0; i<MAX_NIXIE_EVENTS; i++)
    {
        if (m_events[i].event == 0)
        {
            m_events[i].event = event;
            m_events[i].param = param;
            return true;
        }
    }
    return false;
}


bool NixieEvents::put(SNixieEvent &event)
{
    for (uint8_t i=0; i<MAX_NIXIE_EVENTS; i++)
    {
        if (m_events[i].event == 0)
        {
            m_events[i] = event;
            return true;
        }
    }
    return false;
}

SNixieEvent NixieEvents::get()
{
    SNixieEvent event = m_events[0];
    for (uint8_t i=0; i<MAX_NIXIE_EVENTS - 1; i++)
    {
        m_events[i] = m_events[i+1];
    }
    m_events[MAX_NIXIE_EVENTS - 1].event = 0;
    return event;
}
    
bool NixieEvents::hasEvent()
{
    return m_events[0].event != 0;
}

