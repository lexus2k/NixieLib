#include <nixie_display.h>
#include <nixie_k155id1.h>
#include <nixie_library.h>

/*
 * In this example both Nixie Tubes are controlled through single K155ID1 chip.
 * To correctly control tubes digits, we need to turn on tubes in different time 
 * by enabling and disabling power on tube anods.
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
const byte          anodPins[]    =  { 3,   4 };

/*
 * Create display object, consisting of 2 Nixie Tubes
 */
NixieDisplay        g_display(g_driver, anodPins, in14driverMap, 2);

void setup()
{
    Serial.begin(9600);
    g_display.init();
    /* Start with zero-digit. Index corresponds to the tube, we're referring to. */
    g_display[0] = 0;
    g_display[1] = 0;
    /* Activate display object by calling turnOn() method.
     * Display object will automatically call anodOn() and anodOff(), while
     * functioning. Refer to NixieDisplay::render() method
     */
    g_display.turnOn();
}

void loop()
{
    static unsigned long timestamp = millis();
    /* The line below must be called in the beginning of each loop cycle */
    NixieLibrary::update();
    if (millis() - timestamp > 1000)
    {
        /* enumerate all digits one by one every 1000 mseconds on first tube only */
        /* Tube with index 1 should display zero all the time */
        if (g_display[0] <=8)
            g_display[0] = g_display[0] + 1;
        else
            g_display[0] = 0;
        Serial.println(g_display[0].value());
        timestamp = millis();
    }
    /* We call this method each cycle. Also, this method can be placed to interrupt handler */
    g_display.render();
}
