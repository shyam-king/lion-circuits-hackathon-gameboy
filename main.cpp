#include "engine.h"
#include "test.h"
#include <iostream>

#define PAGE_COUNT 8
#define COLUMNS 128
#define MAX_CHANGES PAGE_COUNT*COLUMNS

int main() {
    GameDisplayState gameState;
    GameController *gameController = new TestGame(&gameState);
    ScreenPageChange changes[MAX_CHANGES];
    GameOutputState output;
    GameInputState input;
    for (int i = 0; i < 100; i++) {
        input.left_push_button = false;
        input.up_button = false;
        input.down_button = false;
        input.analog_dial = 0;


        gameController->update_frame(&input, changes, &output);

        std::cout << "Buzzer value: " << (int)output.buzzerValue << std::endl;

        std::cout << "Changes flushed: " << (int)output.screenPageChanges << std::endl;
        for (unsigned int i = 0; i < output.screenPageChanges; i++) {
            std::cout << "Page: " << (int)changes[i].page << ", Column: " << (int)changes[i].column << ", Data: " << (int)changes[i].data << std::endl;
        }
    }
    
    delete gameController;
    return 0;
}