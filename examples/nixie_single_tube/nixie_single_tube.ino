#include <nixie_tube.h>
#include <nixie_k155id1.h>
#include <nixieos.h>

/*
 * In this example single Nixie Tubes is controlled through single K155ID1 chip.
 * To provide power for Nixie tube, please, use step-up converter available in
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
 * Create display object, consisting of single Nixie Tube
 */
NixieTube           g_tube(&g_driver, in14driverMap);

void setup()
{
    NixieOs::setup( nullptr );
    Serial.begin(9600);
    g_tube.init();
    /* Start with zero-digit. Zero means that we start from 0-digit */
    g_tube = 0;
    /* Activate tube object by calling anodOn() method.
     * Call this method even if pin to control tube anod is not used.
     */
    g_tube.anodOn();
}

void loop()
{
    static unsigned long timestamp = millis();
    /* The line below must be called in the beginning of each loop cycle */
    NixieOs::refresh();
    if (millis() - timestamp > 1000)
    {
        /* enumerate all digits one by one every 1000 mseconds */
        if (g_tube <=8)
            g_tube = g_tube + 1;
        else
            g_tube = 0;
        Serial.println(g_tube.value());
        timestamp = millis();
    }
    g_tube.update();
}
