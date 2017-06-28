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

#ifndef _NIXIEOS_TIMER_H_
#define _NIXIEOS_TIMER_H_

#include "nixieos_types.h"

namespace NixieOs
{

enum
{
    TIMER_0,
    TIMER_1
};

/**
 * @brief resets timer, and it starts to count again
 *
 * resets timer counter and it starts from zero.
 *
 * @param id - unique id of timer.
 * @note NixieOs supports 2 timers: 0 and 1.
 */
void resetTimer(uint8_t id);

/**
 * @brief stops and disables timer.
 *
 * stops and disables timer.
 *
 * @param id - unique id of timer.
 * @note NixieOs supports 2 timers: 0 and 1.
 */
void stopTimer(uint8_t id);

/**
 * @brief enables and starts timer.
 *
 * enables and starts timer. When timeoutMs time
 * passes, timer generates NIXIEOS_EVENT_TIMEOUT
 * event.
 *
 * @param id - unique id of timer.
 * @param timeoutMs - timeout in milliseconds
 * @note NixieOs supports 2 timers: 0 and 1.
 */
void startTimer(uint8_t id, uint32_t timeoutMs);

/**
 * @brief restarts disabled timer.
 *
 * restarts disabled timer. When timeoutMs time
 * passes, timer generates NIXIEOS_EVENT_TIMEOUT
 * event.
 *
 * @param id - unique id of timer.
 * @note NixieOs supports 2 timers: 0 and 1.
 */
void restartTimer(uint8_t id);

}

#endif

