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

#ifndef _NIXIEOS_TASKS_H_
#define _NIXIEOS_TASKS_H_

#include "nixieos_events.h"

#define TASK_ID_INVALID 0xFF

#define NIXIEOS_TASK(id, x)  { id, \
                               &x ## EnterFunction, \
                               &x ## StateFunction, \
                               &x ## EventFunction }

#define NIXIEOS_TASK_(id, x)  { id, \
                                nullptr, \
                                &x ## StateFunction, \
                                &x ## EventFunction }

#define NIXIEOS_TASK_END     { TASK_ID_INVALID, nullptr, nullptr, nullptr }


/**
 * Describes single state of NixieOs
 */
typedef struct
{
    /// unique id of the state
    uint8_t id;
    /// state entry function
    void  (*enterCb)();
    /// state main function
    void  (*stateCb)();
    /// state event processing function
    void  (*eventCb)(SNixieEvent &event);
    
} SNixieStateInfo;

#endif