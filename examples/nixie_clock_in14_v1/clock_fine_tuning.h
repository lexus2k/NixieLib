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
#ifndef _CLOCK_FINE_TUNING_H_
#define _CLOCK_FINE_TUNING_H_

/** Default night level for the sensor in ADC units */
#define LS_DEFAULT_NIGHT_LEVEL   160
/** Default room level for the sensor in ADC units */
#define LS_DEFAULT_ROOM_LEVEL    400
/** Default day level for the sensor in ADC units */
#define LS_DEFAULT_DAY_LEVEL     850
/** Light level to go to silent mode [0-32] */
#define LS_SILENT_CONDITIONS     8
/** Light level to go to sleep mode  [0-32] */
#define LS_SLEEP_CONDITIONS      4


/** Alarm ring duration */
#define ALARM_RING_DURATION_MS   60000
/** Minutes to wake up clocks before Alarm in minutes */
#define ALARM_WAKE_UP_CLOCK_BEFORE_M  60
/** Timeout before going to silent mode in milliseconds */
#define GO_TO_SILENT_TIMEOUT_MS  60000
/** Timeout before going to sleep mode in milliseconds */
#define GO_TO_SLEEP_TIMEOUT_MS  60000



/** Timeout to automatically leave setup mode */
#define SETUP_INACTIVITY_TIMEOUT_MS 20000
/** Temperature screen timeout */
#define TEMPERATURE_SCREEN_TIMEOUT_MS 30000
/** Time screen timeout */
#define TIME_SCREEN_TIMEOUT_MS  20000
/** Date screen timeout */
#define DATE_SCREEN_TIMEOUT_MS  10000

/** Period of update diorama brightness */
#define DIORAMA_UPDATE_PERIOD_MS    60000
/** Number of period before diorama lights power off */
#define DIORAMA_LIGHTS_OFF_TIMEOUT_PERIODS 10
/** Minimum brightness of Outdoor lights (0-32) */
#define OUTDOOR_LIGHT_MIN_BRIGHTNESS   12
/** Maximum brightness of Outdoor lights (0-32) */
#define OUTDOOR_LIGHT_MAX_BRIGHTNESS   32

#define OUTDOOR_LIGHT_THRESHOLD_LOW    (8)
#define OUTDOOR_LIGHT_THRESHOLD_HIGH   (13)
#define INDOOR_LIGHT_THRESHOLD_LOW     (6)
#define INDOOR_LIGHT_THRESHOLD_HIGH    (16)


#endif
