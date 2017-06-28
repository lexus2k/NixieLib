/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of Nixie Os Library.

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

#ifndef _NIXIEOS_EVENTS_H_
#define _NIXIEOS_EVENTS_H_

#include "nixieos_types.h"

#define MAX_NIXIE_EVENTS    4

enum
{
    NIXIEOS_EVENT_NONE,
    /// Timeout event is thrown by NixieOs timers.
    NIXIEOS_EVENT_TIMEOUT,
    /// All user events must have numbers equal or above this value
    NIXIEOS_USER_EVENTS = 32,
};

/// SNixieEvent describes content of single event
typedef struct
{
    /// number of event, occured.
    uint8_t event;
    /// additional parameter. For example, for timer event this is timer #.
    uint8_t param;
} SNixieEvent;

/**
 * Class encapsulates work with events.
 */
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

