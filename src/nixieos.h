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

#ifndef _NIXIEOS_H_
#define _NIXIEOS_H_

#include "nixieos_time.h"
#include "nixieos_timer.h"
#include "nixieos_tasks.h"
#include "nixieos_events.h"
#include "nixieos_booster.h"
#include "nixieos_types.h"

/**
 * NixieOs service functions
 */
namespace NixieOs
{

/**
 * @brief Initializes state machine states information.
 *
 * Intializes state machine states info and
 * internal NixieOs variables.
 *
 * @note Array of the states description must be defined
 * as PROGMEM array (located in flash memory). This is
 * required to reduce SRAM consumption.
 *
 * @param tasks - pointer to state machine states array.
 */
void setup(const SNixieStateInfo  tasks[]);

/**
 * @brief Starts infinite loop.
 *
 * Enters infinite loop and runs state machine. If you want
 * to do anything else outside state machine functions, please,
 * do not use this function. Refer to run() function instead.
 *
 */
void loop();

/**
 * @brief Runs single iteration of state machine.
 *
 * Runs single iteration of state machine and exists.
 */
void run();

/**
 * @brief refreshes internal variables of NixieOs.
 *
 * Refreshes internal variables of NixieOs. If you don't
 * call run() for a long time, or you implement infinite/
 * long loop in state machine function, just call refresh()
 * function in the loop to keep all NixieOs service function
 * working.
 */
void refresh();

/**
 * @brief remembers current state and switches to new state.
 *
 * Remembers current state number in the NixieOs stack and
 * switches to new state. This function calls SSSEnterFunction(),
 * where SSS is state name.
 *
 * @param id - state unique number.
 *
 * @see popTask
 */
void pushTask(uint8_t id);

/**
 * @brief switches back to remembered state.
 *
 * Switches to remembered state. This function calls SSSEnterFunction(),
 * where SSS is state name.
 *
 * @see pushTask
 */
void popTask();

/**
 * @brief switches to new state.
 *
 * Switches to new state. This function calls SSSEnterFunction(),
 * where SSS is state name.
 *
 * @param id - state unique number.
 */
void switchTask(uint8_t id);

/**
 * @brief puts new event to internal NixieOs queue.
 *
 * Puts new event to internal NixieOs queue. The events
 * from internal queue are peeked by run() method and
 * are passed to SSSEventFunction() if the currently
 * active state.
 *
 * @param id - event unique id
 * @param param - additional parameter for the event.
 * @return true if event is put to queue
 *         false if there is no space in internal event queue.
 */
bool sendEvent(uint8_t id, uint8_t param);

}

#endif
