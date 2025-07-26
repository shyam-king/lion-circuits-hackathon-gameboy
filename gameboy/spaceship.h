#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <stdint.h>

#include "engine.h"

class Spaceship : public GameController {
    private:
    GameDisplayState *gameState;
   public:
    Spaceship(GameDisplayState *gameState);
    ~Spaceship() throw();
    void handle_input(uint8_t input_code, uint8_t input_value);
    void update_frame();
};

#endif