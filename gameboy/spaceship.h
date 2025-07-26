#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <cstddef>
#include <stdint.h>

#include "engine.h"

#define MAP_WIDTH 1024
#define MAP_HEIGHT 1024

#define DISPLAY_CENTER_X 128/2
#define DISPLAY_CENTER_Y 64/2

#define DIRECTION_RESOLUTION 8

#define MAP_OBJECT_TYPE_BULLET 0
#define MAP_OBJECT_TYPE_BOUNDARY 1
#define MAP_OBJECT_TYPE_ASTEROID 2

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
    void draw_bullet(MapObject &object);
    void clear_bullet(MapObject &object);
    void draw_asteroid(MapObject &object);
    void clear_asteroid(MapObject &object);
    void update_objects();
    void update_bullet(MapObject &object);
    void update_asteroid(MapObject &object);
    void draw_objects();
    void clear_objects();

    void create_bullet(uint8_t direction);
    void update_player_position(GameInputState *input);
    void create_asteroid(uint16_t x, uint16_t y, uint8_t direction);
    void death_screen();

    void set_buzzer(uint8_t value);

    bool check_bullet_collision_for_asteroid(MapObject &object);

    void clean_up_objects();

    void init_map();
    
    MapObject *objects;
    MapObject *objectsSwap;
    size_t object_count;

    protected:
    uint16_t player_x;
    uint16_t player_y;
    uint8_t player_direction;

    bool cleared_screen;
    bool player_hit;

    uint8_t buzzer_value;

   public:
    Spaceship(GameDisplayState *gameState);
    ~Spaceship() throw();
    void update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState);
};

#endif