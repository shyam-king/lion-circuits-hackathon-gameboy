#include "spaceship.h"
#include <cstdint>
#include <cmath>

// #define GAME_DEBUG

#ifdef GAME_DEBUG
#include <iostream>
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#define MAX_OBJECTS 100

#define PLAYER_SPEED 4
#define BULLET_SPEED 5
#define ASTEROID_SPEED 2
#define ASTEROID_RADIUS 3

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

uint8_t scale_input(uint8_t input, uint8_t resolution) {
    uint8_t val = input * resolution / 255;
    if (val > resolution) {
        val = resolution;
    }
    return val;
}


double distance_to_point(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void transform_point(uint16_t x, uint16_t y, uint16_t pivot_x, uint16_t pivot_y, uint16_t *new_x, uint16_t *new_y) {
    // pivot_x and pivot_y are points corresponding to SCREEN_CENTER_X and SCREEN_CENTER_Y
    *new_x = DISPLAY_CENTER_X + (x - pivot_x);
    *new_y = DISPLAY_CENTER_Y + (y - pivot_y);
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
    this->player_x = MAP_WIDTH/2;
    this->player_y = MAP_HEIGHT / 2;
    this->player_direction = 0;
    this->cleared_screen = false;

    this->objects = new MapObject[MAX_OBJECTS];
    this->objectsSwap = new MapObject[MAX_OBJECTS];
    this->object_count = 0;
    this->player_hit = false;
    this->buzzer_value = 0;

    this->init_map();

    DEBUG_PRINT("Game initialized");
}

void Spaceship::init_map() {
    this->create_asteroid(MAP_WIDTH / 2 + 120, ASTEROID_RADIUS*2, 4);
    this->create_asteroid(MAP_WIDTH / 2 - 120, ASTEROID_RADIUS*2, 0);
    this->create_asteroid(MAP_WIDTH  - ASTEROID_SPEED*3, ASTEROID_RADIUS*10, 1);
    this->create_asteroid(ASTEROID_RADIUS*2, ASTEROID_RADIUS*2, 1);
    this->create_asteroid(ASTEROID_RADIUS*2, MAP_HEIGHT/2, 1);
    this->create_asteroid(MAP_WIDTH /2, ASTEROID_RADIUS*10, 3);
    this->create_asteroid(MAP_WIDTH/2, MAP_HEIGHT-ASTEROID_RADIUS*2, 5);
}

Spaceship::~Spaceship() throw() {
    delete[] this->objects;
    delete[] this->objectsSwap;
    this->object_count = 0;
}

bool Spaceship::check_bullet_collision_for_asteroid(MapObject &object) {
    for (size_t i=0; i<this->object_count; i++) {
        if (this->objects[i].type == MAP_OBJECT_TYPE_BULLET) {
            double bullet_x = this->objects[i].x;
            double bullet_y = this->objects[i].y;
            double asteroid_x = object.x;
            double asteroid_y = object.y;

            double bullet_angle = this->objects[i].direction * 2 * M_PI / DIRECTION_RESOLUTION;
            double asteroid_angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;

            double bullet_speed = BULLET_SPEED;
            double asteroid_speed = ASTEROID_SPEED;

            double relative_speed = sqrt(bullet_speed*bullet_speed + asteroid_speed*asteroid_speed - 2*bullet_speed*asteroid_speed*cos(bullet_angle - asteroid_angle));
            double relative_angle = acos((bullet_speed*bullet_speed + relative_speed*relative_speed - asteroid_speed*asteroid_speed) / (2*bullet_speed*relative_speed));
            
            double theta = 2*M_PI - relative_angle;

            double distance = distance_to_point(bullet_x, bullet_y, asteroid_x, asteroid_y);

            double distance_from_center = abs(distance * sin(theta));
            double distance_to_collision_point = abs(distance * cos(theta));

            DEBUG_PRINT("distance: " << distance);
            DEBUG_PRINT("distance_from_center: " << distance_from_center);
            DEBUG_PRINT("distance_to_collision_point: " << distance_to_collision_point);

            if (distance_from_center <= ASTEROID_RADIUS && distance_to_collision_point <= relative_speed) {
                DEBUG_PRINT("bullet hit asteroid");
                return true;
            }
        }
    }
    return false;
}

void Spaceship::clean_up_objects() { //objects that need to be deleted from mem (bullets)
    size_t new_object_count = 0;
    for (size_t i=0; i<this->object_count; i++) {
        if (this->objects[i].type == MAP_OBJECT_TYPE_ASTEROID) {
            if (distance_to_point(this->player_x, this->player_y, this->objects[i].x, this->objects[i].y) < 10) {
                this->player_hit = true;
                continue; //asteroid hit player
            }

            if (this->check_bullet_collision_for_asteroid(objects[i])) {
                this->set_buzzer(100);
                this->clear_asteroid(objects[i]);
                continue; //bullet hit asteroid
            }
        }

        if (this->objects[i].x < 0 || this->objects[i].x >= MAP_WIDTH || this->objects[i].y < 0 || this->objects[i].y >= MAP_HEIGHT) {
            continue; //bullet out of bounds
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
        this->set_buzzer(50);
    }
}

void Spaceship::create_asteroid(uint16_t x, uint16_t y, uint8_t direction) {
    if (this->object_count < MAX_OBJECTS) {
        this->objects[this->object_count].x = x;
        this->objects[this->object_count].y = y;
        this->objects[this->object_count].direction = direction;
        this->objects[this->object_count].type = MAP_OBJECT_TYPE_ASTEROID;
        this->object_count++;

        DEBUG_PRINT("asteroid created: " << x << ", " << y << " direction: " << (int)direction << " new object count: " << (int)this->object_count);
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


void Spaceship::clear_bullet(MapObject &object) {
    for (uint16_t x=MAX(object.x-2, 0); x<=MIN(object.x+2, MAP_WIDTH); x++) {
        for (uint16_t y=MAX(object.y-2, 0); y<=MIN(object.y+2, MAP_HEIGHT); y++) {
            uint16_t screen_x, screen_y;
            transform_point(x, y, this->player_x, this->player_y, &screen_x, &screen_y);
            this->gameState->update_screen_pixel(screen_x, screen_y, false);
        }
    }
}

void Spaceship::draw_bullet(MapObject &object) {
    uint16_t pivot_x = this->player_x;
    uint16_t pivot_y = this->player_y;

    for (uint16_t x=MAX(object.x-1, 0); x<=MIN(object.x+1, MAP_WIDTH); x++) {
        uint16_t y = object.y;
        uint16_t screen_x, screen_y;
        uint16_t rotated_x, rotated_y;
        get_rotated_point(x, y, object.direction, &rotated_x, &rotated_y, object.x, object.y);
        transform_point(rotated_x, rotated_y, this->player_x, this->player_y, &screen_x, &screen_y);
        this->gameState->update_screen_pixel(screen_x, screen_y, true);
    }
}

void Spaceship::update_objects() {
    for (size_t i=0; i<this->object_count; i++) {
        switch (this->objects[i].type) {
            case MAP_OBJECT_TYPE_BULLET:
                this->update_bullet(this->objects[i]);
                break;
            case MAP_OBJECT_TYPE_ASTEROID:
                this->update_asteroid(this->objects[i]);
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

    DEBUG_PRINT("bullet: " << object.x << ", " << object.y << " direction: " << (int)object.direction);
}

void Spaceship::update_asteroid(MapObject &object) {
    double angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;
    uint16_t new_x = object.x + ASTEROID_SPEED * cos(angle);
    uint16_t new_y = object.y + ASTEROID_SPEED * sin(angle);

    DEBUG_PRINT("asteroid new: " << new_x << ", " << new_y << " direction: " << (int)object.direction);

    //collisions
    if (new_x < 0) {
        DEBUG_PRINT("asteroid hit left wall");
        new_x = ASTEROID_SPEED;
        double angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;
        angle = M_PI - angle;
        if (angle < 0) {
            angle = 2 * M_PI + angle;
        }
        object.direction = (uint8_t)(angle * DIRECTION_RESOLUTION / (2 * M_PI));
    } else if (new_x >= MAP_WIDTH) {
        DEBUG_PRINT("asteroid hit right wall");
        new_x = MAP_WIDTH - ASTEROID_SPEED;
        double angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;
        angle = M_PI - angle;
        if (angle < 0) {
            angle = 2 * M_PI + angle;
        }
        object.direction = (uint8_t)(angle * DIRECTION_RESOLUTION / (2 * M_PI));
    } else if (new_y < 0) {
        DEBUG_PRINT("asteroid hit top wall");
        new_y = ASTEROID_SPEED;
        double angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;
        angle = angle - 3*M_PI/2;
        if (angle < 0) {
            angle = 2 * M_PI + angle;
        }
        object.direction = (uint8_t)(angle * DIRECTION_RESOLUTION / (2 * M_PI));
    } else if (new_y >= MAP_HEIGHT) {
        DEBUG_PRINT("asteroid hit bottom wall");
        new_y = MAP_HEIGHT - ASTEROID_SPEED;
        double angle = object.direction * 2 * M_PI / DIRECTION_RESOLUTION;
        angle = M_PI/2-angle;
        if (angle < 0) {
            angle = 2 * M_PI + angle;
        }
        object.direction = (uint8_t)(angle * DIRECTION_RESOLUTION / (2 * M_PI));
    }

    object.x = new_x;
    object.y = new_y;

    DEBUG_PRINT("asteroid: " << object.x << ", " << object.y << " direction: " << (int)object.direction);
}

void Spaceship::clear_asteroid(MapObject &object) {
    //clear asteroid
    for (uint16_t x=MAX(object.x-ASTEROID_RADIUS, 0); x<=MIN(object.x+ASTEROID_RADIUS, MAP_WIDTH); x++) {
        for (uint16_t y=MAX(object.y-ASTEROID_RADIUS, 0); y<=MIN(object.y+ASTEROID_RADIUS, MAP_HEIGHT); y++) {
            uint16_t screen_x, screen_y;
            transform_point(x, y, this->player_x, this->player_y, &screen_x, &screen_y);
            this->gameState->update_screen_pixel(screen_x, screen_y, false);
        }
    }
}

void Spaceship::draw_asteroid(MapObject &object) {   
    //draw asteroid
    for (uint16_t x=MAX(object.x-ASTEROID_RADIUS, 0); x<=MIN(object.x+ASTEROID_RADIUS, MAP_WIDTH); x++) {
        for (uint16_t y=MAX(object.y-ASTEROID_RADIUS, 0); y<=MIN(object.y+ASTEROID_RADIUS, MAP_HEIGHT); y++) {
            double distance = distance_to_point(x, y, object.x, object.y);
            if (abs(distance-ASTEROID_RADIUS) < 0.5) {
                uint16_t screen_x, screen_y;
                transform_point(x, y, this->player_x, this->player_y, &screen_x, &screen_y);
                this->gameState->update_screen_pixel(screen_x, screen_y, true);
            }
        }
    }
}

void Spaceship::draw_player() {
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

void Spaceship::clear_objects() {
    for (size_t i=0; i<this->object_count; i++) {
        switch (this->objects[i].type) {
            case MAP_OBJECT_TYPE_BULLET:
                this->clear_bullet(this->objects[i]);
                break;
            case MAP_OBJECT_TYPE_ASTEROID:
                this->clear_asteroid(this->objects[i]);
                break;
        }
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

void Spaceship::death_screen() {
    for (uint16_t x=0; x<128; x++) {
        for (uint16_t y=0; y<64; y++) {
            this->gameState->update_screen_pixel(x, y, true);
        }
    }
}

void Spaceship::set_buzzer(uint8_t value) {
    if (this->buzzer_value < value) {
        this->buzzer_value = value;
    }
}

void Spaceship::update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState) {
    if (!this->player_hit) {
        //clean up
        this->clean_up_objects();

        // Drawing 
        if (!this->cleared_screen) {
            this->gameState->clear_screen();
            this->cleared_screen = true;
        }

        this->clear_player();
        this->player_direction = scale_input(inputState->analog_dial, DIRECTION_RESOLUTION);

        this->clear_objects();
        
        this->buzzer_value = 0;
        this->update_player_position(inputState);

        if (inputState->left_push_button) {
            this->create_bullet(this->player_direction);
        }


        this->update_objects();

        this->draw_player();
        this->draw_objects();

        DEBUG_PRINT("player: " << this->player_x << ", " << this->player_y << " direction: " << (int)this->player_direction);
    } else {
        this->death_screen();
        this->set_buzzer(255);
    } 

    uint16_t changes_flushed = this->gameState->flush_screen_changes(changes);

    outputState->screenPageChanges = changes_flushed;
    outputState->buzzerValue = this->buzzer_value;
}
