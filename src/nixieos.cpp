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

#include "nixieos.h"

// SECTION ======================= TIME ==================================

/*   8 bytes    */
uint32_t  g_nixieUsEx = 0;
uint32_t  g_nixieMsEx = 0;

NixieOsTime  __currentTime;

namespace NixieOs
{

/*
void setTimeEx(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    __currentTime.hours   = hours;
    __currentTime.minutes = minutes;
    __currentTime.seconds = seconds;
}

void setTime(NixieOsTime timeRec)
{
    __currentTime = timeRec;
}

void getTime(NixieOsTime *timeRec)
{
    *timeRec = __currentTime;
} */

/**
 * the function is intended for internal use only
 */
void updateTime() __attribute__((weak));
void updateTime()
{
    
}


}


// SECTION ======================= EVENTS ================================

/*   8 bytes    */
static SNixieEvent s_events[MAX_NIXIE_EVENTS];

namespace NixieOs
{

bool sendEvent(uint8_t id, uint8_t param)
{
    for (uint8_t i=0; i<MAX_NIXIE_EVENTS; i++)
    {
        if (s_events[i].event == 0)
        {
            s_events[i].event = id;
            s_events[i].param = param;
            return true;
        }
    }
    return false;
}

}

// SECTION ======================= TASKS =================================


void defaultEventHandler(SNixieEvent &event) __attribute__((weak));
void defaultEventHandler(SNixieEvent &event)
{
}


static void emptyEnterFunction() {};
static void emptyStateFunction() {};
static void emptyEventFunction(SNixieEvent &event) { defaultEventHandler(event); };
static void emptyExitFunction() {};

/*   (2+2+2+1) + 2 + 1 = 10 bytes    */
static SNixieStateInfo         s_activeTask = NIXIEOS_TASK(TASK_ID_INVALID, empty);
static const SNixieStateInfo  *s_tasks;
static uint8_t                 s_popTask;

namespace NixieOs
{

void pushTask(uint8_t id)
{
    s_popTask = s_activeTask.id;
    switchTask(id);
}

void popTask()
{
    switchTask(s_popTask);
}

void switchTask(uint8_t id)
{
    uint8_t i=0;
    for(;;)
    {
        uint8_t nextId = pgm_read_byte(&s_tasks[i].id);
        if (nextId == TASK_ID_INVALID)
        {
            break;
        }
        if (nextId == id)
        {
            s_activeTask.id  = nextId;
            s_activeTask.enterCb = pgm_read_ptr(&s_tasks[i].enterCb);
            s_activeTask.stateCb = pgm_read_ptr(&s_tasks[i].stateCb);
            s_activeTask.eventCb = pgm_read_ptr(&s_tasks[i].eventCb);
            if (s_activeTask.enterCb)
            {
                s_activeTask.enterCb();
            }
            break;
        }
        i++;
    };
}


}

// SECTION ======================= TIMERS =================================

#define NIXIEOS_MAX_TIMERS    2
/* 9*2 = 18 bytes */

/**
 * Describes NixieOs timer state
 */
typedef struct
{
    /// true if timer is active
    bool      aimed;
    /// timestamp of timer activation
    uint32_t  startMs;
    /// timeout in milliseconds
    uint32_t  timeoutMs;
} STimerInfo;

static STimerInfo s_timers[NIXIEOS_MAX_TIMERS];

namespace NixieOs
{

void resetTimer(uint8_t id)
{
    s_timers[id].startMs = g_nixieMsEx;
}


void stopTimer(uint8_t id)
{
    s_timers[id].aimed = false;
}


void startTimer(uint8_t id, uint32_t timeoutMs)
{
    s_timers[id].aimed = true;
    s_timers[id].startMs = g_nixieMsEx;
    s_timers[id].timeoutMs = timeoutMs;
}

void restartTimer(uint8_t id)
{
    s_timers[id].aimed = true;
    s_timers[id].startMs = g_nixieMsEx;
}

}


// SECTION ======================= ENGINE =================================

void initNixieBooster();

namespace NixieOs
{

void setup(const SNixieStateInfo  tasks[])
{
    initNixieBooster();
    s_tasks = tasks;
    refresh();
}


void loop()
{
    for(;;)
    {
        run();
    }
}


void refresh()
{
    g_nixieMsEx = millis();  
    g_nixieUsEx = micros();
}


void run()
{
    uint8_t   lms = (uint8_t)g_nixieMs;
    g_nixieMsEx = millis();  
    g_nixieUsEx = micros();
    /* Recalculate timers only when milliseconds counter changes */
    if (lms != (uint8_t)g_nixieMs)
    {
        for(uint8_t id=0; id<NIXIEOS_MAX_TIMERS; id++)
        {
            if (s_timers[id].aimed)
            {
                if (g_nixieMsEx - s_timers[id].startMs > s_timers[id].timeoutMs)
                {
                    NixieOs::sendEvent(NIXIEOS_EVENT_TIMEOUT, id);
                    s_timers[id].aimed = false;
                }
            }
        }
        if (lms ^ (uint8_t)g_nixieMs & 0x80)
        {
            updateTime();
        }
    }
    if (s_events[0].event != 0)
    {
        SNixieEvent event = s_events[0];
        for (uint8_t i=0; i<MAX_NIXIE_EVENTS - 1; i++)
        {
            s_events[i] = s_events[i+1];
        }
        s_events[MAX_NIXIE_EVENTS - 1].event = 0;
        s_activeTask.eventCb(event);
    }
    s_activeTask.stateCb();
}

}

