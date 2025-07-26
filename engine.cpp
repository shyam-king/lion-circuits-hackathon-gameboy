#include "engine.h"
#include <cstdint>

// #define GAME_DEBUG

#ifdef GAME_DEBUG
#include <iostream>
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#define COLUMNS 128
#define PAGE_COUNT 8

GameDisplayState::GameDisplayState() {
    this->resolution_x = COLUMNS;
    this->resolution_y = PAGE_COUNT*8;
    this->screen_buffer = new unsigned char[PAGE_COUNT*COLUMNS]();
    this->edited_page_mask_buffer = new unsigned char[COLUMNS*PAGE_COUNT/8](); // 1 bit per column per page

    DEBUG_PRINT("GameState initialized");
}

GameDisplayState::~GameDisplayState() throw() {
    delete[] this->screen_buffer;
    delete[] this->edited_page_mask_buffer;
}

void GameDisplayState::clear_screen() {
   for (unsigned char x = 0; x < this->resolution_x; x++) {
    for (unsigned char y = 0; y < this->resolution_y; y++) {
        this->update_screen_pixel(x, y, false);
    }
   }
}

void GameDisplayState::update_screen_pixel(unsigned char x, unsigned char y, bool state) {
    unsigned char column = x;
    unsigned char page = y / 8;
    unsigned char row = y % 8;
    unsigned char bit = 1 << row;
    DEBUG_PRINT("updating bit at page " << (int)page << " column " << (int)column << " to " << (int)bit);
    if (state) {
        this->screen_buffer[page * COLUMNS + column] |= bit;
    } else {
        this->screen_buffer[page * COLUMNS + column] &= ~bit;
    }

    unsigned char mask_index = column * PAGE_COUNT/8 + page/8;
    unsigned char mask_bit = 1 << (page % 8);
    DEBUG_PRINT("setting mask at index " << (int)mask_index << " to " << (int)mask_bit);
    this->edited_page_mask_buffer[mask_index] |= mask_bit;
}

uint8_t GameDisplayState::flush_screen_changes(ScreenPageChange *changes) {
    uint8_t changes_flushed = 0;
    for (unsigned char page = 0; page < PAGE_COUNT; page++) {
        for (unsigned char column = 0; column < COLUMNS; column++) {
            unsigned char mask = this->edited_page_mask_buffer[column * PAGE_COUNT/8 + page/8] & (1 << (page % 8));
            if (mask != 0) {
                changes[changes_flushed].page = page;
                changes[changes_flushed].column = column;
                changes[changes_flushed].data = this->screen_buffer[page * COLUMNS + column];
                changes_flushed++;

                this->edited_page_mask_buffer[column * PAGE_COUNT/8 + page/8] &= ~mask; // clear the mask for this column and page
            }
        }
    }

    return changes_flushed;
}

int GameDisplayState::test() {
    return 1;
}

GameController::GameController(GameDisplayState *gameState) {
    this->gameState = gameState;
}


