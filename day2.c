#include "arraylist.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum direction {
    FORWARD, UP, DOWN
} direction;

typedef struct command {
    direction direction;
    int amount;
} command;

int main() {
    arraylist commands;
    arraylist_init(&commands, sizeof(command));
    for (;;) {
        char direction_str[8];
        int amount;
        int result = scanf("%7s %d\n", direction_str, &amount);
        if (result == EOF) {
            break;
        }
        assert(result == 2);

        direction direction;
        if (strcmp(direction_str, "forward") == 0) direction = FORWARD;
        else if (strcmp(direction_str, "up") == 0) direction = UP;
        else if (strcmp(direction_str, "down") == 0) direction = DOWN;
        else assert(0);

        arraylist_append(&commands, &(command){ .direction = direction, .amount = amount });
    }

    int depth = 0;
    int position = 0;
    for (command* command = arraylist_begin(&commands); command != arraylist_end(&commands); ++command) {
        switch (command->direction) {
            case FORWARD: position += command->amount; break;
            case UP: depth -= command->amount; break;
            case DOWN: depth += command->amount; break;
            default: assert(0);
        }
    }
    printf("%d\n", depth * position);

    depth = 0;
    position = 0;
    int aim = 0;
    for (command* command = arraylist_begin(&commands); command != arraylist_end(&commands); ++command) {
        switch (command->direction) {
            case FORWARD: position += command->amount; depth += aim * command->amount; break;
            case UP: aim -= command->amount; break;
            case DOWN: aim += command->amount; break;
            default: assert(0);
        }
    }
    printf("%d\n", depth * position);

    arraylist_free(&commands);
    return 0;
}
