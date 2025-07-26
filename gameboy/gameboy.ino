#include <Wire.h>
#include "display.h"
#include "engine.h"
#include "spaceship.h"

#define BUZZER_PIN 0
#define ACCELERATOR 2
#define BRAKE 3
#define SHOOT 1
#define POT_PIN A0

GameDisplayState *displayState;
GameController *spaceship;
ScreenPageChange *changes;

bool screen_state = false;
bool buzzer_state = false;

void setup()
{
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(ACCELERATOR, INPUT_PULLUP);
    pinMode(BRAKE, INPUT_PULLUP);
    pinMode(SHOOT, INPUT_PULLUP);
    displayState = new GameDisplayState();
    spaceship = new Spaceship(displayState);
    changes = new ScreenPageChange[8 * 128 + 1];

    Serial.begin(115200);
    Wire.setClock(400000); // Fast I2C
    Wire.begin();          // Use custom pins

    initSSD1306(); // Initialize SSD1306
    clearScreen();
    // drawSmiley();                // Fill screen with pattern
}

void loop()
{
    int potValue = analogRead(POT_PIN);

    bool left_push_button_pressed = digitalRead(SHOOT) == LOW;
    bool up_button_pressed = digitalRead(ACCELERATOR) == LOW;
    bool down_button_pressed = digitalRead(BRAKE) == LOW;

    // char buf[120]; // Increased buffer size to fit pot value

    // sprintf(buf,
    //         "Left: %s | Up: %s | Down: %s | Pot: %d",
    //         left_push_button_pressed ? "ON" : "OFF",
    //         up_button_pressed ? "ON" : "OFF",
    //         down_button_pressed ? "ON" : "OFF",
    //         potValue);

    // // Serial.println(buf);

    GameInputState input;
    GameOutputState output;

    input.up_button = up_button_pressed;
    input.down_button = down_button_pressed;
    input.left_push_button = left_push_button_pressed;
    input.analog_dial = potValue/16;

    spaceship->update_frame(&input, changes, &output);

    // Serial.println(output.screenPageChanges);

    applyUpdates(changes, output.screenPageChanges);
    ringBuzzer(output.buzzerValue);

    delay(1000);
}

void ringBuzzer(uint8_t intensity)
{
    analogWrite(BUZZER_PIN, intensity); // 0 = silent, 255 = full volume
}