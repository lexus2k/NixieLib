#include <nixie_tube.h>
#include <nixie_k155id1.h>
#include <nixie_library.h>


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
    Serial.begin(9600);
    g_tube.init();
    /* Start with zero-digit. Zero index means the single tube we have */
    g_tube = 0;
    g_tube.anodOn();
}

void loop()
{
    static unsigned long timestamp = millis();
    /* The line below must be called in the beginning of each loop cycle */
    NixieLibrary::update();
    if (millis() - timestamp > 1000)
    {
        /* enumerate all digits one by one every 1000 mseconds */
        if (g_tube >=8)
            g_tube = g_tube + 1;
        else
            g_tube = 0;
        Serial.println(g_tube.value());
        timestamp = millis();
    }
    g_tube.update();
}
