#include "engine.h"
#include <stdint.h>

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
    this->screen_buffer = new uint8_t[PAGE_COUNT*COLUMNS]();
    this->edited_page_mask_buffer = new uint8_t[COLUMNS*PAGE_COUNT/8](); // 1 bit per column per page

    DEBUG_PRINT("GameState initialized");
}

GameDisplayState::~GameDisplayState() throw() {
    delete[] this->screen_buffer;
    delete[] this->edited_page_mask_buffer;
}

void GameDisplayState::clear_screen() {
   for (uint8_t x = 0; x < this->resolution_x; x++) {
    for (uint8_t y = 0; y < this->resolution_y; y++) {
        this->update_screen_pixel(x, y, false);
    }
   }
}

void GameDisplayState::update_screen_pixel(uint8_t x, uint8_t y, bool state) {
    DEBUG_PRINT("updating pixel " << (int)x << ", " << (int)y << " for game state " << (void*)this);
    uint8_t column = x;
    uint8_t page = y / 8;
    uint8_t row = y % 8;
    uint8_t bit = 1 << row;
    DEBUG_PRINT("updating bit at page " << (int)page << " column " << (int)column << " to " << (int)bit);


    if (x >= this->resolution_x || y >= this->resolution_y || x < 0 || y < 0) {
        DEBUG_PRINT("pixel out of bounds");
        return;
    }

    uint8_t current_value = this->screen_buffer[page * COLUMNS + column] & bit;
    if (current_value == state) {
        return;
    }

    if (state) {
        this->screen_buffer[page * COLUMNS + column] |= bit;
    } else {
        this->screen_buffer[page * COLUMNS + column] &= ~bit;
    }

    uint8_t mask_index = column * PAGE_COUNT/8 + page/8;
    uint8_t mask_bit = 1 << (page % 8);
    DEBUG_PRINT("setting mask at index " << (int)mask_index << " to " << (int)mask_bit);
    this->edited_page_mask_buffer[mask_index] |= mask_bit;
}

uint16_t GameDisplayState::flush_screen_changes(ScreenPageChange *changes) {
    uint16_t changes_flushed = 0;
    for (uint8_t page = 0; page < PAGE_COUNT; page++) {
        for (uint8_t column = 0; column < COLUMNS; column++) {
            uint8_t mask = this->edited_page_mask_buffer[column * PAGE_COUNT/8 + page/8] & (1 << (page % 8));
            DEBUG_PRINT("checking mask for page " << (int)page << " column " << (int)column << " mask " << (int)mask);
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


GameController::GameController(GameDisplayState *gameState) {
    this->gameState = gameState;
    DEBUG_PRINT("GameController constructor");
    DEBUG_PRINT("gameState: " << (void*)(this->gameState) << " " << (void*)gameState);
}

GameController::~GameController() throw() {
    // this->gameState = 0;
}

void GameDisplayState::render_screen_as_str(char* str) {
    uint16_t str_index = 0;
    for (uint8_t y = 0; y < this->resolution_y; y++) {
        for (uint8_t x = 0; x < this->resolution_x; x++) {
            uint8_t column = x;
            uint8_t page = y / 8;
            uint8_t row = y % 8;
            uint8_t bit = 1 << row;

            uint8_t current_value = this->screen_buffer[page * COLUMNS + column] & bit;
            str[str_index++] = current_value ? '.' : ' ';
        }
        str[str_index++] = '\n';
    }
    str[str_index] = '\0';
}


