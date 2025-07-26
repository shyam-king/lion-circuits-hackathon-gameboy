#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <stdint.h>

#include "engine.h"

#define MAP_WIDTH 1024
#define MAP_HEIGHT 1024

#define DISPLAY_CENTER_X 128/2
#define DISPLAY_CENTER_Y 64/2

#define DIRECTION_RESOLUTION 16

#define MAP_OBJECT_TYPE_BULLET 0
#define MAP_OBJECT_TYPE_BOUNDARY 1

struct MapObject {
    uint16_t x;
    uint16_t y;
    uint8_t direction;
    uint8_t type;
};

class Spaceship : public GameController {
    private:
    void draw_player();
    void clear_player();
    void get_rotated_point(uint16_t x, uint16_t y, uint8_t direction, uint16_t *rotated_x, uint16_t *rotated_y);
    void create_bullet();
    void draw_bullets();
    
    MapObject *objects;
    uint8_t object_count;

    protected:
    uint16_t player_x;
    uint16_t player_y;
    uint8_t player_direction;

    bool cleared_screen;

   public:
    Spaceship(GameDisplayState *gameState);
    ~Spaceship() throw();
    void update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState);
};

#endif