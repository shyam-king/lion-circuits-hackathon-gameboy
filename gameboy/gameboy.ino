#include <Wire.h>
#include "display.h"

#define SDA_PIN 8
#define SCL_PIN 9

void setup()
{
    Serial.begin(115200);
    Wire.setClock(400000);             // Fast I2C
    Wire.begin();      // Use custom pins

    initSSD1306();                     // Initialize SSD1306
    clearScreen();
    drawSmiley();                // Fill screen with pattern
}


void loop()
{
    // Nothing here yet
}