#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include "engine.h"

#define SSD1306_ADDR 0x3C // SSD1306 I2C address

// Public interface
void initSSD1306();
void writeCommand(uint8_t cmd);
void writeCommands(const uint8_t *cmds, uint8_t count);
void writeData(const uint8_t *data, uint8_t count);
void setCursor(uint8_t page, uint8_t column);
void applyUpdates(ScreenPageChange *updates, size_t count);
void clearScreen();
void drawSmiley();

#endif /
