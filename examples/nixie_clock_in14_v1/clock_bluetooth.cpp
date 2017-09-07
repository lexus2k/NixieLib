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

#include "clock_bluetooth.h"
#include "nixie_clock.h"
#include "clock_lightsensor.h"
#include "clock_nightmode.h"

#include <SoftwareSerial.h>

static SoftwareSerial  g_btSerial(14,16);  // 14 RX, 16 TX

void bluetoothInit()
{
    g_btSerial.begin(9600);
    g_btSerial.print("AT+NAMECLOCK");
    delay(100);
    while (g_btSerial.available()) g_btSerial.read();
/*    g_btSerial.print("AT+BAUD6"); // 38400
    delay(500);
    while (g_btSerial.available()) g_btSerial.read();
    g_btSerial.end();
    g_btSerial.begin(38400);*/

}


static int16_t toInt(uint8_t *buf, uint8_t & index, uint8_t len)
{
    int16_t a = 0;
    while (index < len)
    {
        if ((buf[index]<'0') || (buf[index]>'9'))
        {
            break;
        }
        a = a * 10 + (buf[index]-'0');
        index++;
    }
    return a;
}


void bluetoothCheck()
{
    if (g_btSerial.available())
    {
        /* PowerOff display, since we cannot process it correctly. */
        g_display.powerOff();
        /* Sleep N mseconds to have enough time to receive 16 bytes at 9600baud */
        delay( ((16 + 2) * 1000) / (9600/8) );
        /* Consider that no incoming bytes on RX line. SoftwareSerial cannot receive and send at the same time */
        uint8_t buf[16];
        uint8_t index = 0;
        while (g_btSerial.available())
        {
            buf[index++] = g_btSerial.read();
        }
        switch(buf[0])
        {
            case 'i':
                g_audioClient.inDoorLight((buf[1] - '0')*25);
                break;
            case 'o':
                g_audioClient.outDoorLight((buf[1] - '0')*25);
                break;
            case 'l':
                g_btSerial.print(g_sensor.absoluteValue());
                break;
            case 'b':
                g_btSerial.print(g_sensor.brightness());
                break;
            case 's':
                if (buf[2]=='f') g_display.scrollOff();
                if (buf[2]=='n') g_display.scrollOn();
                break;
            case 'd':
                if (index < 4)
                {
                    g_btSerial.print( g_savedSettings.dayLevel );
                }
                else
                {
                    uint8_t i = 1;
                    g_savedSettings.dayLevel = toInt(buf, i, index);
                    saveToEeprom();
                    g_btSerial.write( "OK" );
                }
                break;
            case 'r':
                if (index < 4)
                {
                    g_btSerial.print( g_savedSettings.roomLevel );
                }
                else
                {
                    uint8_t i = 1;
                    g_savedSettings.roomLevel = toInt(buf, i, index);
                    saveToEeprom();
                    g_btSerial.write( "OK" );
                }
                break;
            case 'n':
                if (index < 4)
                {
                    g_btSerial.print( g_savedSettings.nightLevel );
                }
                else
                {
                    uint8_t i = 1;
                    g_savedSettings.nightLevel = toInt(buf, i, index);
                    saveToEeprom();
                    g_btSerial.write( "OK" );
                }
                break;
            case 't':
                if (index < 8)
                {
                    g_btSerial.write( "ERR" );
                }
                else if (buf[1] != 'd')
                {
                    uint8_t i = 1;
                    g_rtc.m_hours = Ds3231::toInternal( toInt(buf, i, index) );
                    i++;
                    g_rtc.m_minutes = Ds3231::toInternal( toInt(buf, i, index) );
                    i++;
                    g_rtc.m_seconds = Ds3231::toInternal( toInt(buf, i, index) );
                    g_rtc.setTime();
                }
                else
                {
                    uint8_t i = 2;
                    g_rtc.m_year = Ds3231::toInternal( toInt(buf, i, index) );
                    i++;
                    g_rtc.m_month = Ds3231::toInternal( toInt(buf, i, index) );
                    i++;
                    g_rtc.m_day = Ds3231::toInternal( toInt(buf, i, index) );
                    g_rtc.setDate();
                }
                break;
            case 'h':
            default:
                g_btSerial.write("use 1 char command with args\n");
                g_btSerial.write("io - diorama light\n");
                g_btSerial.write("lb - test light sensor\n");
                g_btSerial.write("son,soff - scroll test\n");
                g_btSerial.write("drn - get/set light sensor settings\n");
                g_btSerial.write("tHH:MM:SS - set time\n");
                g_btSerial.write("tdYYYY/MM/DD - set date\n");
                break;
        }
        if (!isNightSleeping())
        {
             g_display.powerOn();
        }
    }
}