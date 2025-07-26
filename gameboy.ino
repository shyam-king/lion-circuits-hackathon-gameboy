#include <Wire.h>
#include "display.h"

#define SDA_PIN 8
#define SCL_PIN 9

void setup()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    Serial.begin(115200);
    initSH1106();

    // Test pattern updates
    DisplayUpdate updates[] = {
        {0, 0, 0xAA},
        {0, 1, 0xBB},
        {0, 2, 0xCC},
        {2, 1, 0xDD}};

    applyUpdates(updates, 4);
}

void loop()
{
    // Nothing here yet
}