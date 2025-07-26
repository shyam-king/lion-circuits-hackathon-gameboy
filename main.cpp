#include "gameboy/engine.h"
#include "gameboy/spaceship.h"
#include <iostream>

#define PAGE_COUNT 8
#define COLUMNS 128
#define MAX_CHANGES PAGE_COUNT*COLUMNS

int main() {
    GameDisplayState *gameState = new GameDisplayState();
    gameState->update_screen_pixel(0, 0, true);
    GameController *gameController = new Spaceship(gameState);
    ScreenPageChange changes[MAX_CHANGES];
    GameOutputState output;
    GameInputState input;

    char screen_str[128*64*2];

    for (int i = 0; i < 20; i++) {
        input.left_push_button = i == 0;
        input.up_button = false;
        input.down_button = false;
        input.analog_dial = 256-128;


        // std::cout << "Frame " << i << std::endl;
        gameController->update_frame(&input, changes, &output);
        // std::cout << "Frame " << i << " done" << std::endl;

        // std::cout << "Buzzer value: " << (int)output.buzzerValue << std::endl;

        // std::cout << "Changes flushed: " << (int)output.screenPageChanges << std::endl;
        // for (unsigned int i = 0; i < output.screenPageChanges; i++) {
        //     std::cout << "Page: " << (int)changes[i].page << ", Column: " << (int)changes[i].column << ", Data: " << (int)changes[i].data << std::endl;
        // }
        gameState->render_screen_as_str(screen_str);
        std::cout << screen_str << std::endl;
        std::cout << "--------------------------------" << std::endl;
        int x;
        std::cin >> x;
    }
    
    delete gameController;
    delete gameState;
    return 0;
}