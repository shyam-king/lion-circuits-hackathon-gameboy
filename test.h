#ifndef TEST_GAME_H
#define TEST_GAME_H

#include <stdint.h>

#include "engine.h"

class TestGame : public GameController {
    private:
    GameDisplayState *gameState;
    bool flag;
   public:
    TestGame(GameDisplayState *gameState);
    ~TestGame() throw();
    void handle_input(uint8_t input_code, uint8_t input_value);
    void update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState);
};

#endif