#include "spaceship.h"
#include <cstdint>
#include <cmath>

Spaceship::Spaceship(GameDisplayState *gameState): GameController(gameState) {
    this->player_x = MAP_WIDTH / 2;
    this->player_y = MAP_HEIGHT / 2;
    this->player_direction = 0;
    this->cleared_screen = false;

    this->objects = new MapObject[100];
    this->object_count = 0;
}

Spaceship::~Spaceship() throw() {
    delete[] this->objects;
}

void Spaceship::get_rotated_point(uint16_t x, uint16_t y, uint8_t direction, uint16_t *rotated_x, uint16_t *rotated_y) {
    double angle = direction * 2 * M_PI / DIRECTION_RESOLUTION;
    double x1 = x;
    double y1 = y;
    double pivot_x = DISPLAY_CENTER_X;
    double pivot_y = DISPLAY_CENTER_Y;

    double x2 = pivot_x + (x1-pivot_x)*cos(angle) - (y1-pivot_y)*sin(angle);
    double y2 = pivot_y + (x1-pivot_x)*sin(angle) + (y1-pivot_y)*cos(angle);

    *rotated_x = (uint16_t)x2;
    *rotated_y = (uint16_t)y2;
}

void Spaceship::clear_player() {
    for (uint16_t x=DISPLAY_CENTER_X-9; x<=DISPLAY_CENTER_X+9; x++) {
        for (uint16_t y=DISPLAY_CENTER_Y-9; y<=DISPLAY_CENTER_Y+9; y++) {
            this->gameState->update_screen_pixel(x, y, false);
        }
    }
}

void Spaceship::create_bullet() {
    if (this->object_count < 100) {

    }
}

void Spaceship::draw_bullets() {}

void Spaceship::draw_player() {
    this->clear_player();
    for (uint16_t x=DISPLAY_CENTER_X-4; x<=DISPLAY_CENTER_X-3; x++) {
        for (uint16_t y=DISPLAY_CENTER_Y-4; y<=DISPLAY_CENTER_Y+4; y++) {
            uint16_t rotated_x, rotated_y;
            this->get_rotated_point(x, y, this->player_direction, &rotated_x, &rotated_y);
            this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
        }
    }
    //base shrinking lines
    for (uint16_t y=DISPLAY_CENTER_Y-3; y<=DISPLAY_CENTER_Y+3; y++) {
        uint16_t rotated_x, rotated_y;
        this->get_rotated_point(DISPLAY_CENTER_X-2, y, this->player_direction, &rotated_x, &rotated_y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }

    //base shrinking lines
    for (uint16_t y=DISPLAY_CENTER_Y-2; y<=DISPLAY_CENTER_Y+2; y++) {
        uint16_t rotated_x, rotated_y;
        this->get_rotated_point(DISPLAY_CENTER_X-1, y, this->player_direction, &rotated_x, &rotated_y);
        this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
    }

    for (uint16_t x=DISPLAY_CENTER_X; x <= DISPLAY_CENTER_X+3; x++){
        for (uint16_t y=DISPLAY_CENTER_Y-1; y<=DISPLAY_CENTER_Y+1; y++) {
            uint16_t rotated_x, rotated_y;
            this->get_rotated_point(x, y, this->player_direction, &rotated_x, &rotated_y);
            this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
        }
    }

    uint16_t rotated_x, rotated_y;

    this->get_rotated_point(DISPLAY_CENTER_X+4, DISPLAY_CENTER_Y, this->player_direction, &rotated_x, &rotated_y);
    this->gameState->update_screen_pixel(rotated_x, rotated_y, true);
}

void Spaceship::update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState) {
    outputState->buzzerValue = 0;
    outputState->screenPageChanges = 0;

    // Drawing 
    if (!this->cleared_screen) {
        this->gameState->clear_screen();
        this->cleared_screen = true;
    }

    this->draw_player();

    // logics
    if (inputState->left_push_button) {
        this->create_bullet();
    }

    uint16_t changes_flushed = this->gameState->flush_screen_changes(changes);

    this->player_direction += 1;
    if (this->player_direction >= DIRECTION_RESOLUTION) {
        this->player_direction = 0;
    }
}
