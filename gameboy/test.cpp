#include "test.h"
#include "engine.h"

#define RES_X 128
#define RES_Y 64

// #define GAME_DEBUG

#ifdef GAME_DEBUG
#include <iostream>
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#include <stdint.h>

TestGame::TestGame(GameDisplayState *gameState): GameController(gameState) {
    this->flag = false;
    DEBUG_PRINT("TestGame constructor");
}

TestGame::~TestGame() throw() {
    
}


void TestGame::update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState) {
    this->flag = !this->flag;
    for (uint8_t x = 0; x < RES_X; x++) {
        for (uint8_t y = 0; y < RES_Y; y+=2) {
            this->gameState->update_screen_pixel(x, y, this->flag);
        }
    }

    // this->gameState->update_screen_pixel(0, 0, this->flag);

    uint16_t changes_flushed = this->gameState->flush_screen_changes(changes);

    outputState->buzzerValue = this->flag ? 255 : 0;
    outputState->screenPageChanges = changes_flushed;
}
