#include "display.h"

// Send multiple command bytes
void writeCommands(const uint8_t *cmds, uint8_t count)
{
    Wire.beginTransmission(SSD1306_ADDR);
    Wire.write(0x00); // Command mode
    for (uint8_t i = 0; i < count; i++)
    {
        Wire.write(cmds[i]);
    }
    Wire.endTransmission();
}

// Set cursor (page, column) for SSD1306
void setCursor(uint8_t page, uint8_t column)
{
    if (page > 7 || column > 127)
        return;
    uint8_t cmds[] = {
        (uint8_t)(0xB0 | (page & 0x07)),         // Page
        (uint8_t)(0x00 | (column & 0x0F)),       // Lower col
        (uint8_t)(0x10 | ((column >> 4) & 0x0F)) // Higher col
    };
    writeCommands(cmds, 3);
}

// Send data bytes to the current position
void writeData(const uint8_t *data, uint8_t count)
{
    Wire.beginTransmission(SSD1306_ADDR);
    Wire.write(0x40); // Co = 0, D/C# = 1
    for (uint8_t i = 0; i < count; i++)
    {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

// Apply a batch of pixel data updates
void applyUpdates(ScreenPageChange *updates, size_t count)
{
    if (count == 0)
        return;

    size_t i = 0;
    while (i < count)
    {
        uint8_t currentPage = updates[i].page;
        uint8_t startCol = updates[i].column;
        setCursor(currentPage, startCol);

        Wire.beginTransmission(SSD1306_ADDR);
        Wire.write(0x40); // Data mode

        while (i < count)
        {
            if (updates[i].page != currentPage || updates[i].column != startCol)
                break;
            Wire.write(updates[i].data);
            ++i;
            ++startCol;
        }

        Wire.endTransmission();
    }
}

// SSD1306 initialization (horizontal addressing)
void initSSD1306()
{
    delay(100);

    uint8_t initCmds[] = {
        0xAE,       // Display OFF
        0x20, 0x00, // Memory Addressing Mode = Horizontal
        0xB0,       // Page start address
        0xC8,       // COM output scan direction
        0x00,       // Low column addr
        0x10,       // High column addr
        0x40,       // Start line
        0x81, 0x7F, // Contrast
        0xA1,       // Segment remap
        0xA6,       // Normal display
        0xA8, 0x3F, // Multiplex = 64
        0xA4,       // Display from RAM
        0xD3, 0x00, // Display offset
        0xD5, 0x80, // Clock divide
        0xD9, 0xF1, // Pre-charge
        0xDA, 0x12, // COM pins
        0xDB, 0x40, // VCOM detect
        0x8D, 0x14, // Charge pump
        0xAF        // Display ON
    };
    writeCommands(initCmds, sizeof(initCmds));
}

void drawSmiley()
{
    // 8x8 smiley pattern (each byte is a vertical column, bottom-up)
    uint8_t smiley[8] = {
        0x3C, // 00111100
        0x42, // 01000010
        0xA5, // 10100101
        0x81, // 10000001
        0xA5, // 10100101
        0x99, // 10011001
        0x42, // 01000010
        0x3C  // 00111100
    };

    ScreenPageChange updates[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        updates[i] = {2, (uint8_t)(60 + i), smiley[i]};
    }

    applyUpdates(updates, 8);
}

void drawSmileyCentered()
{
    uint8_t smiley[8] = {
        0x3C, 0x42, 0xA5, 0x81,
        0xA5, 0x99, 0x42, 0x3C};

    uint8_t startPage = 3; // Vertically centered (page 3)
    uint8_t startCol = 60; // Horizontally centered (column 60)

    ScreenPageChange updates[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        updates[i] = {startPage, (uint8_t)(startCol + i), smiley[i]};
    }

    applyUpdates(updates, 8);
}

void clearScreen()
{
    ScreenPageChange updates[1024]; // 128 columns × 8 pages = 1024
    size_t index = 0;

    for (uint8_t page = 0; page < 8; page++)
    {
        for (uint8_t col = 0; col < 128; col++)
        {
            updates[index++] = {page, col, 0x00}; // 0x00 = all pixels off
        }
    }

    applyUpdates(updates, index);
}
