#include <Wire.h>
#include "display.h"
#include "engine.h"
#include "spaceship.h"

#define SDA_PIN 8
#define SCL_PIN 9

GameDisplayState *displayState;
GameController *spaceship;
ScreenPageChange *changes;

void setup()
{
    displayState = new GameDisplayState();
    spaceship = new Spaceship(displayState);
    changes = new ScreenPageChange[8*128+1];

    Serial.begin(115200);
    Wire.setClock(400000);             // Fast I2C
    Wire.begin();      // Use custom pins

    initSSD1306();                     // Initialize SSD1306
    clearScreen();
    // drawSmiley();                // Fill screen with pattern
}


void loop()
{
    GameInputState input;
    GameOutputState output;

    Serial.println(1);

    spaceship -> update_frame(&input, changes, &output);

    Serial.println(2);

    applyUpdates(changes, output.screenPageChanges);

    Serial.println(3);
    delay(1000);
}