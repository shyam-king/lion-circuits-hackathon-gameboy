#include "display.h"

void writeCommand(uint8_t cmd)
{
    Wire.beginTransmission(SH1106_ADDR);
    Wire.write(0x00);
    Wire.write(cmd);
    Wire.endTransmission();
}

void writeCommands(const uint8_t *cmds, uint8_t count)
{
    Wire.beginTransmission(SH1106_ADDR);
    Wire.write(0x00);
    for (uint8_t i = 0; i < count; i++)
    {
        Wire.write(cmds[i]);
    }
    Wire.endTransmission();
}

void writeData(const uint8_t *data, uint8_t count)
{
    Wire.beginTransmission(SH1106_ADDR);
    Wire.write(0x40);
    for (uint8_t i = 0; i < count; i++)
    {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

void setCursor(uint8_t page, uint8_t column)
{
    if (page > 7 || column > 127)
        return;

    column += 2; // SH1106 RAM offset

    uint8_t pageCmd = 0xB0 | (page & 0x07);
    uint8_t colLow = 0x00 | (column & 0x0F);
    uint8_t colHigh = 0x10 | ((column >> 4) & 0x0F);

    uint8_t cmds[] = {pageCmd, colLow, colHigh};
    writeCommands(cmds, 3);
}

void initSH1106()
{
    delay(100);
    uint8_t initCmds[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F,
        0xD3, 0x00, 0x40, 0xAD, 0x8B,
        0xA1, 0xC8, 0xDA, 0x12, 0x81,
        0xCF, 0xD9, 0xF1, 0xDB, 0x40,
        0xA4, 0xA6, 0xAF};
    writeCommands(initCmds, sizeof(initCmds));
}

void applyUpdates(DisplayUpdate *updates, size_t count)
{
    if (count == 0)
        return;

    size_t i = 0;
    while (i < count)
    {
        uint8_t currentPage = updates[i].page;
        uint8_t startCol = updates[i].column;

        setCursor(currentPage, startCol);

        Wire.beginTransmission(SH1106_ADDR);
        Wire.write(0x40);

        while (i < count)
        {
            uint8_t nextPage = updates[i].page;
            uint8_t nextCol = updates[i].column;

            if (nextPage != currentPage || nextCol != startCol)
                break;

            Wire.write(updates[i].data);
            ++i;
            ++startCol;
        }

        Wire.endTransmission();
    }
}
