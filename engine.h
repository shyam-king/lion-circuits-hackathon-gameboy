#ifndef ENGINE_H
#define ENGINE_H

#include <cstdint>
#include <stdint.h>

#define INPUT_CODE_LEFT_PUSH_BUTTON 0
#define INPUT_CODE_UP_BUTTON 1
#define INPUT_CODE_DOWN_BUTTON 2
#define INPUT_CODE_ANALOG_DIAL 3

#define INPUT_VALUE_PRESSED 0
#define INPUT_VALUE_RELEASED 1

struct ScreenPageChange {
    uint8_t page;
    uint8_t column;
    uint8_t data;
};

struct GameInputState {
    bool left_push_button;
    bool up_button;
    bool down_button;
    uint8_t analog_dial;
};

struct GameOutputState {
    uint8_t buzzerValue;
    uint8_t screenPageChanges;
};

class GameDisplayState {
    private:
        uint8_t resolution_x;
        uint8_t resolution_y;

        uint8_t *screen_buffer;
        uint8_t *edited_page_mask_buffer;

    public:
        GameDisplayState();
        ~GameDisplayState() throw();
        int test();
        void clear_screen();
        uint8_t flush_screen_changes(ScreenPageChange *changes); // returns the number of changes flushed
        void update_screen_pixel(uint8_t x, uint8_t y, bool state);
};

class GameController {
    private:
    GameDisplayState *gameState;


    public:
    GameController(GameDisplayState *gameState);

    /*
        input_code:
        - INPUT_CODE_LEFT_PUSH_BUTTON
        - INPUT_CODE_UP_BUTTON
        - INPUT_CODE_DOWN_BUTTON
        - INPUT_CODE_ANALOG_DIAL
        input_value:
        - INPUT_VALUE_PRESSED
        - INPUT_VALUE_RELEASED

        - for dial, provide angle value between 0 and 255
    */
    virtual void update_frame(GameInputState *inputState, ScreenPageChange *changes, GameOutputState *outputState) = 0; // update the game state for the next frame [should be called once per frame]

    virtual ~GameController() throw() = 0;
};

#endif

