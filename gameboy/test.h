#ifndef TEST_GAME_H
#define TEST_GAME_H

#include <stdint.h>

#include "engine.h"

class TestGame : public GameController {
    bool flag;
   public:
    TestGame(GameDisplayState *gameState);
    ~TestGame() throw();
    void update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState);
};

#endif