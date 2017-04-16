/*
    Copyright 2017 (C) Alexey Dynda

    This file is part of Nixie Clock Library.

    Protocol Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Protocol Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _NIXIE_EVENTS_H_
#define _NIXIE_EVENTS_H_

#include "nixie_types.h"

#define MAX_NIXIE_EVENTS    3

/// SNixieEvent describes content of single event
typedef struct
{
    uint8_t event;
    uint8_t param;
} SNixieEvent;

class NixieEvents
{
public:
    /**
     * Builds new events fifo container
     */
    NixieEvents();

    /**
     * Puts event to events container
     * @param event - application specific event
     * @param param - parameter related to event
     * @return true if successful
     *         false if events container is full.
     */
    bool put(uint8_t event, uint8_t param);

    /**
     * Puts event to events container
     * @param event - application specific event
     * @return true if successful
     *         false if events container is full.
     */
    bool put(SNixieEvent &event);

    /**
     * Reads event from events container
     * @return SNixieEvent structure.
     * @see hasEvent()
     */
    SNixieEvent get();

    /**
     * Returns true if there are events in events container
     */
    bool hasEvent();
private:
    SNixieEvent m_events[MAX_NIXIE_EVENTS];
};

#endif

