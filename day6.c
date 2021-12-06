#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned long count_fishies(unsigned long initial_state[9], int ticks) {
    unsigned long state_a[9];
    unsigned long state_b[9];
    memcpy(state_a, initial_state, sizeof(state_a));

    unsigned long *state = state_a;
    unsigned long *next_state = state_b;
    for (int i = 0; i < ticks; ++i) {
        next_state[0] = state[1];
        next_state[1] = state[2];
        next_state[2] = state[3];
        next_state[3] = state[4];
        next_state[4] = state[5];
        next_state[5] = state[6];
        next_state[6] = state[7] + state[0];
        next_state[7] = state[8];
        next_state[8] = state[0];
        unsigned long *tmp = state;
        state = next_state;
        next_state = tmp;
    }

    return state[0] + state[1] + state[2] + state[3] + state[4] + state[5] + state[6] + state[7] + state[8];
}

int main() {
    unsigned long state[9] = {0};
    for (;;) {
        int fish;
        int result = scanf("%d,", &fish);
        if (result == EOF) {
            break;
        }
        assert(result == 1);
        assert(0 <= fish && fish <= 8);
        state[fish] += 1;
    }

    printf("%lu\n", count_fishies(state, 80));
    printf("%lu\n", count_fishies(state, 256));

    return 0;
}
