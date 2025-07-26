#include "spaceship.h"
#include <cstdint>
#include <cmath>

#define MAX_OBJECTS 65335

#define PLAYER_SPEED 10
#define BULLET_SPEED 50

uint8_t scale_input(uint8_t input, uint8_t resolution) {
    uint8_t val = input * resolution / 255;
    if (val > resolution) {
        val = resolution;
    }
    return val;
}

void get_rotated_point(uint16_t x, uint16_t y, uint8_t direction, uint16_t *rotated_x, uint16_t *rotated_y, uint16_t pivot_xx = DISPLAY_CENTER_X, uint16_t pivot_yy = DISPLAY_CENTER_Y) {
    double angle = direction * 2 * M_PI / DIRECTION_RESOLUTION;
    double x1 = x;
    double y1 = y;
    double pivot_x = pivot_xx;
    double pivot_y = pivot_yy;

    double x2 = pivot_x + (x1-pivot_x)*cos(angle) - (y1-pivot_y)*sin(angle);
    double y2 = pivot_y + (x1-pivot_x)*sin(angle) + (y1-pivot_y)*cos(angle); 

    *rotated_x = (uint16_t)x2;
    *rotated_y = (uint16_t)y2;
}

Spaceship::Spaceship(GameDisplayState *gameState): GameController(gameState) {
    this->player_x = MAP_WIDTH / 2;
    this->player_y = MAP_HEIGHT / 2;
    this->player_direction = 0;
    this->cleared_screen = false;

    this->objects = new MapObject[MAX_OBJECTS];
    this->objectsSwap = new MapObject[MAX_OBJECTS];
    this->object_count = 0;
}

Spaceship::~Spaceship() throw() {
    delete[] this->objects;
    delete[] this->objectsSwap;
    this->object_count = 0;
}

void Spaceship::clean_up_objects() {
    size_t new_object_count = 0;
    for (size_t i=0; i<this->object_count; i++) {
        if (this->objects[i].x < 0 || this->objects[i].x >= MAP_WIDTH || this->objects[i].y < 0 || this->objects[i].y >= MAP_HEIGHT) {
            continue;
        }

        this->objectsSwap[new_object_count] = this->objects[i];
        new_object_count++;
    }

    this->object_count = new_object_count;
    MapObject *temp = this->objects;
    this->objects = this->objectsSwap;
    this->objectsSwap = temp;
}

void Spaceship::clear_player() {
    for (uint16_t x=DISPLAY_CENTER_X-5; x<=DISPLAY_CENTER_X+7; x++) {
        for (uint16_t y=DISPLAY_CENTER_Y-5; y<=DISPLAY_CENTER_Y+5; y++) {
            this->gameState->update_screen_pixel(x, y, false);
        }
    }
}

void Spaceship::create_bullet(uint8_t direction) {
    if (this->object_count < MAX_OBJECTS) {
        this->objects[this->object_count].x = this->player_x;
        this->objects[this->object_count].y = this->player_y;
        this->objects[this->object_count].direction = direction;
        this->objects[this->object_count].type = MAP_OBJECT_TYPE_BULLET;
        this->object_count++;
    }
}

void Spaceship::create_asteroid(uint16_t x, uint16_t y, uint8_t direction) {
    if (this->object_count < MAX_OBJECTS) {
        this->objects[this->object_count].x = x;
        this->objects[this->object_count].y = y;
        this->objects[this->object_count].direction = direction;
        this->objects[this->object_count].type = MAP_OBJECT_TYPE_ASTEROID;
        this->object_count++;
    }
}

void Spaceship::draw_objects() {
    for (size_t i=0; i<this->object_count; i++) {
        switch (this->objects[i].type) {
            case MAP_OBJECT_TYPE_BULLET:
                this->draw_bullet(this->objects[i]);
                break;
            case MAP_OBJECT_TYPE_ASTEROID:
                this->draw_asteroid(this->objects[i]);
                break;
        }
    }
}



double distance_to_point(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void Spaceship::draw_bullet(MapObject &object) {
    uint16_t pivot_x = this->player_x;
    uint16_t pivot_y = this->player_y;

    for (uint16_t x=-1; x<=1; x++) {
        uint16_t point_x = object.x + x + pivot_x;
        uint16_t point_y = object.y + pivot_y;
        uint16_t rotated_x, rotated_y;
        get_rotated_point(point_x, point_y, object.direction, &rotated_x, &rotated_y, object.x, object.y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }
}

void Spaceship::update_objects() {
    for (size_t i=0; i<this->object_count; i++) {
        switch (this->objects[i].type) {
            case MAP_OBJECT_TYPE_BULLET:
                this->update_bullet(this->objects[i]);
                break;
        }
    }
}

void Spaceship::update_bullet(MapObject &object) {
    double angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;
    uint16_t new_x = object.x + BULLET_SPEED * cos(angle);
    uint16_t new_y = object.y + BULLET_SPEED * sin(angle);

    object.x = new_x;
    object.y = new_y;
}

void Spaceship::draw_asteroid(MapObject &object) {
    uint16_t pivot_x = this->player_x;
    uint16_t pivot_y = this->player_y;

    
    for (uint16_t x=-4; x<=4; x++) {
        for (uint16_t y=-4; y<=4; y++) {
            double distance = distance_to_point(pivot_x, pivot_y, object.x, object.y);
            if (abs(distance-4) < 0.1) {
                uint16_t point_x = object.x + x + pivot_x;
                uint16_t point_y = object.y + y + pivot_y;
                this->gameState->update_screen_pixel(point_x, point_y, true);
            }
        }
    }
}

void Spaceship::draw_player() {
    this->clear_player();
    for (uint16_t x=DISPLAY_CENTER_X-5; x<=DISPLAY_CENTER_X-4; x++) {
        for (uint16_t y=DISPLAY_CENTER_Y-5; y<=DISPLAY_CENTER_Y+5; y++) {
            uint16_t rotated_x, rotated_y;
            get_rotated_point(x, y, this->player_direction, &rotated_x, &rotated_y);
            this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
        }
    }
    //base shrinking lines
    for (uint16_t y=DISPLAY_CENTER_Y-4; y<=DISPLAY_CENTER_Y+4; y++) {
        uint16_t rotated_x, rotated_y;
        get_rotated_point(DISPLAY_CENTER_X-3, y, this->player_direction, &rotated_x, &rotated_y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }

    for (uint16_t y=DISPLAY_CENTER_Y-3; y<=DISPLAY_CENTER_Y+3; y++) {
        uint16_t rotated_x, rotated_y;
        get_rotated_point(DISPLAY_CENTER_X-2, y, this->player_direction, &rotated_x, &rotated_y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }

    for (uint16_t y=DISPLAY_CENTER_Y-2; y<=DISPLAY_CENTER_Y+2; y++) {
        uint16_t rotated_x, rotated_y;
        get_rotated_point(DISPLAY_CENTER_X-1, y, this->player_direction, &rotated_x, &rotated_y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }

    for (uint16_t x=DISPLAY_CENTER_X; x <= DISPLAY_CENTER_X+4; x++){
        for (uint16_t y=DISPLAY_CENTER_Y-1; y<=DISPLAY_CENTER_Y+1; y++) {
            uint16_t rotated_x, rotated_y;
            get_rotated_point(x, y, this->player_direction, &rotated_x, &rotated_y);
            this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
        }
    }

    for (uint16_t x=DISPLAY_CENTER_X+5; x <= DISPLAY_CENTER_X+7; x++){
        uint16_t rotated_x, rotated_y;
        get_rotated_point(x, DISPLAY_CENTER_Y, this->player_direction, &rotated_x, &rotated_y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }
}

void Spaceship::update_player_position(GameInputState *input) {
    if (!input->down_button && !input->up_button) {
        return;
    }

    uint8_t direction = this->player_direction;
    double angle = direction * 2 * M_PI / DIRECTION_RESOLUTION;

    if (input->down_button) {
        angle -= M_PI;
    }

    uint16_t new_x = this->player_x + PLAYER_SPEED * cos(angle);
    uint16_t new_y = this->player_y + PLAYER_SPEED * sin(angle);

    if (new_x < 0) {
        new_x = 0;
    }

    if (new_x >= MAP_WIDTH) {
        new_x = MAP_WIDTH - 1;
    }

    if (new_y < 0) {
        new_y = 0;
    }

    if (new_y >= MAP_HEIGHT) {
        new_y = MAP_HEIGHT - 1;
    }

    this->player_x = new_x;
    this->player_y = new_y;
}

void Spaceship::update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState) {
    //updates
    this->update_player_position(inputState);

    if (inputState->left_push_button) {
        this->create_bullet(this->player_direction);
    }

    this->player_direction = scale_input(inputState->analog_dial, DIRECTION_RESOLUTION);

    this->update_objects();

    //clean up
    this->clean_up_objects();

    // Drawing 
    if (!this->cleared_screen) {
        this->gameState->clear_screen();
        this->cleared_screen = true;
    }

    this->draw_player();
    this->draw_objects();

    uint16_t changes_flushed = this->gameState->flush_screen_changes(changes);

    this->player_direction += 1;
    if (this->player_direction >= DIRECTION_RESOLUTION) {
        this->player_direction = 0;
    }
    
    outputState->screenPageChanges = changes_flushed;
}
