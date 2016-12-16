#include <nixie_display.h>
#include <nixie_k155id1.h>


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
NixieDisplay        g_display(g_driver, nullptr, in14driverMap, 1);

void setup()
{
    Serial.begin(9600);
    g_display.init();
    g_display.turnOn();
    /* Start with zero-digit. Zero index means the single tube we have */
    g_display[0] = 0;
}

void loop()
{
    static unsigned long timestamp = millis();
    if (millis() - timestamp > 1000)
    {
        if (g_display[0] >=8)
            g_display[0] = g_display[0] + 1;
        else
            g_display[0] = 0;
        Serial.println(g_display[0].value());
        timestamp = millis();
    }
    g_display.render(micros());   
}
