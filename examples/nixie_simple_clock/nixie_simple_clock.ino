#include <nixie_display.h>
#include <nixie_k155id1.h>
#include <nixieos.h>

/*
 * In this example four Nixie Tubes are controlled through single K155ID1 chip.
 * To correctly control tubes digits, we need to turn on tubes in different time 
 * by enabling and disabling power on tube anods.
 * The example demonstrates how to implement code for simple nixie clock without
 * any controls. Just clocks counting since 00:00. You can simply improve this
 * example and add seconds.
 * To provide power for Nixie tubes, please, use step-up converter available in
 * schematics folder. It transforms 12V to 180V, required to power up Nixie tubes.
 * Don't forget that tube anod should be connected via 12K resistor (0.25W is good)
 * to 180V.
 */

/*
 * Assume that I1, I2, I3 and I4 input pins of K155ID1 are connected to
 * 7, 12, 13 and 8th digital pins of Arduino accordingly
 */
const byte    driverPins[]  =  { 7, 12,  13,  8 };

K155ID1             g_driver(driverPins);
/*
 * For now, consider that output lines of K155ID1 are connected to corresponding 
 * pins of Nixie Tube (IN-14). For example, O0 pin of K155ID1 is connected to
 * tube pin, responsible for digit 0, and so on.
 */
const byte          in14driverMap[10]  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

/*
 * anodPins array contains pins, which are used to control power voltage on Nixie Tube Anods.
 * Lets assume, these pins are 3, 4
 */
//                                   MIN L   MIN H  HOUR L  HOUR H
const byte          anodPins[]    =  { 3,      4,      5,      6 };

/*
 * Create display object, consisting of 2 Nixie Tubes
 */
NixieDisplay        g_display(g_driver, anodPins, in14driverMap, 4);

// Define variable to hold current time
NixieOsTime currentTime = { 00, 00, 00 };
// Define variable to hold millis() timestamp, when we last updated the currentTime
uint32_t    lastTimeUpdateMs;

void showCurrentTime()
{
    g_display[0] = currentTime.minutes % 10;
    g_display[1] = currentTime.minutes / 10;
    g_display[2] = currentTime.hours % 10;
    g_display[3] = currentTime.hours / 10;
}

void setup()
{
    NixieOs::setup( nullptr );
    lastTimeUpdateMs = g_nixieMsEx; // or you can use millis() here if you want
    g_display.init();
    /* Activate display object by calling powerOn() method.
     * Display object will automatically call anodOn() and anodOff() for each tube, while
     * functioning. Refer to NixieDisplay::render() method
     */
    g_display.powerOn();
    showCurrentTime();
}

void loop()
{
    /* The line below must be called in the beginning of each loop cycle */
    NixieOs::run();
    /* Arduino millis(), which is used by NixieOs is not accurate, this means *
     * each 1000 milliseconds of millis() is actually 1024 milliseconds in    *
     * real world. So, to compensate that we update current time every 976.56 *
     * virtual milliseconds. */
    if (g_nixieMsEx - lastTimeUpdateMs >= 977)
    {
        lastTimeUpdateMs += 977;
        if ( ++currentTime.seconds == 60 )
        {
            currentTime.seconds = 0;
            if ( ++currentTime.minutes == 60 )
            {
                currentTime.minutes = 0;
                if ( ++currentTime.hours == 24 )
                {
                    currentTime.hours = 0;
                }
            }
            /* We don't have seconds in our clock, so update display, *
             * when only minutes being changed. Use special effect */
            g_display.scroll321( );
            showCurrentTime();
        }
    }
    /* We call this method each cycle. */
    g_display.render();
}
