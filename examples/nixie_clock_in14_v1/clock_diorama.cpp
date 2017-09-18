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

#include "clock_diorama.h"
#include "nixie_clock.h"
#include "clock_brightness.h"
#include "clock_events.h"
#include "clock_lightsensor.h"
#include "clock_fine_tuning.h"


static uint8_t outdoorLightsOn    = 0;
static uint8_t outdoorLightsTimeout;
static uint8_t indoorLightsOn     = 0;
static uint8_t indoorLightsTimeout;
static bool outdoorLightEnable = true;
static bool indoorLightEnable  = true;

void enableDioramaLights(bool enable)
{
    outdoorLightEnable = enable;
    indoorLightEnable  = enable;
}

static inline bool inDoorLightsTime()
{
    return g_sensor.brightness() >= INDOOR_LIGHT_THRESHOLD_LOW
           && g_sensor.brightness() <= INDOOR_LIGHT_THRESHOLD_HIGH
           && indoorLightEnable;
}

static inline bool notInDoorLightsTime()
{
    return g_sensor.brightness() < INDOOR_LIGHT_THRESHOLD_LOW
           || g_sensor.brightness() > INDOOR_LIGHT_THRESHOLD_HIGH
           || !indoorLightEnable;
}


static inline bool outDoorLightsTime()
{
    return g_sensor.brightness() > OUTDOOR_LIGHT_THRESHOLD_LOW
           && g_sensor.brightness() < OUTDOOR_LIGHT_THRESHOLD_HIGH
           && outdoorLightEnable;
}

static inline bool notOutDoorLightsTime()
{
    return g_sensor.brightness() > (OUTDOOR_LIGHT_THRESHOLD_HIGH + 1)
           || !outdoorLightEnable
           || g_sensor.brightness() < (OUTDOOR_LIGHT_THRESHOLD_LOW -1);
}

static uint8_t calcOutdoorBrightness()
{
    uint8_t brightness = g_sensor.brightness() + OUTDOOR_LIGHT_MIN_BRIGHTNESS;
    brightness = max(brightness, OUTDOOR_LIGHT_MIN_BRIGHTNESS);
    brightness = min(brightness, OUTDOOR_LIGHT_MAX_BRIGHTNESS);
    return brightness;
}

void updateDioramaLights()
{
    if (0 == outdoorLightsOn)
    {
        if (outDoorLightsTime())
        {
            outdoorLightsOn = calcOutdoorBrightness();
            outdoorLightsTimeout = DIORAMA_LIGHTS_OFF_TIMEOUT_PERIODS;
            g_audioClient.outDoorLight(7 * outdoorLightsOn);
            NixieOs::startTimer(NixieOs::TIMER_1, DIORAMA_UPDATE_PERIOD_MS);
        }
    }
    if (0 == indoorLightsOn)
    {
        if (inDoorLightsTime())
        {
            indoorLightsOn = 1;
            indoorLightsTimeout = DIORAMA_LIGHTS_OFF_TIMEOUT_PERIODS;
            g_audioClient.inDoorLight(255);
            NixieOs::startTimer(NixieOs::TIMER_1, DIORAMA_UPDATE_PERIOD_MS);
        }
    }
}

bool handleDioramaEvents(SNixieEvent &event)
{
    if ( event.event == NIXIEOS_EVENT_TIMEOUT && event.param == NixieOs::TIMER_1 )
    {
        if (notOutDoorLightsTime())
        {
            if (outdoorLightsTimeout)
            {
                outdoorLightsTimeout--;
            }
            if (0 == outdoorLightsTimeout)
            {
                outdoorLightsOn = 0;
            }
        }
        else
        {
            outdoorLightsTimeout = DIORAMA_LIGHTS_OFF_TIMEOUT_PERIODS;
        }
        if (notInDoorLightsTime())
        {
            if (indoorLightsTimeout)
            {
                indoorLightsTimeout--;
            }
            if (0 == indoorLightsTimeout)
            {
                indoorLightsOn = 0;
            }
        }
        else
        {
            indoorLightsTimeout = DIORAMA_LIGHTS_OFF_TIMEOUT_PERIODS;
        }
        if (!indoorLightsOn)
        {
            g_audioClient.inDoorLight(0);
        }
        if (outdoorLightsOn)
        {
            outdoorLightsOn = calcOutdoorBrightness();
            g_audioClient.outDoorLight( 7 * outdoorLightsOn);
        }
        else
        {
            g_audioClient.outDoorLight( 0 );
        }
        if (outdoorLightsOn || indoorLightsOn)
        {
            NixieOs::restartTimer(event.param);
        }
        return true;
    }
    return false;
}
