#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>

#define SH1106_ADDR 0x3C // Default I2C address for SH1106

struct DisplayUpdate
{
    uint8_t page;
    uint8_t column;
    uint8_t data;
};

// Public interface
void initSH1106();
void writeCommand(uint8_t cmd);
void writeCommands(const uint8_t *cmds, uint8_t count);
void writeData(const uint8_t *data, uint8_t count);
void setCursor(uint8_t page, uint8_t column);
void applyUpdates(DisplayUpdate *updates, size_t count);

#endif /
