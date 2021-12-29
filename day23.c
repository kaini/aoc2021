#include "heap.h"
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int min(int a, int b) { return a < b ? a : b; }

static int max(int a, int b) { return a > b ? a : b; }

static int energy[4] = {1, 10, 100, 1000};

#define HALLWAY_SIZE 11
#define ROOM_COUNT 4
typedef struct state {
    int room_size;
    char state[];
} state;

static size_t state_size(const state *state) { return HALLWAY_SIZE + ROOM_COUNT * state->room_size; }

static char *hallway(state *state) { return state->state; }

static char *room(state *state, int r) { return state->state + HALLWAY_SIZE + r * state->room_size; }

static bool state_eq(const void *a, const void *b) {
    const state *aa = a;
    const state *bb = b;
    assert(aa->room_size == bb->room_size);
    return memcmp(aa->state, bb->state, state_size(aa)) == 0;
}

static size_t state_hash(const void *a) {
    const state *aa = a;
    size_t hash = 0;
    for (size_t i = 0; i < state_size(aa); ++i) {
        hash |= i * 255 * aa->state[i];
    }
    return hash;
}

static bool hallway_free(state *state, int a, int b) {
    for (int h = min(a, b) + 1; h <= max(a, b) - 1; ++h) {
        if (hallway(state)[h] != '.') {
            return false;
        }
    }
    return true;
}

static int estimate_cost(state *state) {
    int cost = 0;
    for (int r = 0; r < ROOM_COUNT; ++r) {
        for (int i = 0; i < state->room_size; ++i) {
            if (room(state, r)[i] != '.') {
                int amphi = room(state, r)[i] - 'A';
                if (amphi != r) {
                    cost += energy[amphi] * ((i + 1) /*out*/ + abs(r - amphi) * 2 /*hallway*/ + 1 /*in*/);
                }
            }
        }
    }
    return cost;
}

static int find_free_cell(state *state, int r) {
    for (int i = state->room_size - 1; i >= 0; --i) {
        if (room(state, r)[i] == '.') {
            return i;
        }
    }
    return -1;
}

static int find_move_cell(state *state, int r) {
    for (int i = 0; i < state->room_size; ++i) {
        if (room(state, r)[i] != '.') {
            return i;
        }
    }
    return -1;
}

static bool move_makes_sense(state *state, int r, int i) {
    for (; i < state->room_size; ++i) {
        if (room(state, r)[i] - 'A' != r) {
            return true;
        }
    }
    return false;
}

static int search(state *initial_state, const state *goal_state) {
    const bool btrue = true;

    heap frontier;
    heap_init(&frontier, sizeof(struct state) + state_size(initial_state), state_hash, state_eq);
    map settled;
    map_init(&settled, sizeof(struct state) + state_size(initial_state), state_hash, state_eq, sizeof(bool));

    heap_insert_or_decrease(&frontier, initial_state, estimate_cost(initial_state));

    state *state = malloc(sizeof(struct state) + state_size(initial_state));
    int cost;
    size_t last_print = 0;
    while (heap_pull(&frontier, state, &cost)) {
        cost -= estimate_cost(state);

        if (map_size(&settled) >= last_print + 5000) {
            last_print = map_size(&settled);
            fprintf(stderr, "cost @ %d frontier=%zd settled=%zd\n", cost + estimate_cost(state), heap_size(&frontier),
                    map_size(&settled));
        }

        if (state_eq(state, goal_state)) {
            free(state);
            heap_free(&frontier);
            map_free(&settled);
            return cost;
        }

        map_put(&settled, state, &btrue);

        // move from the hallway into a room
        for (int h = 0; h < HALLWAY_SIZE; ++h) {
            // check if there is something to move
            if (hallway(state)[h] != '.') {
                int goal = hallway(state)[h] - 'A';
                assert(0 <= goal && goal < 4);
                // check if the goal room is free
                int room_cell = find_free_cell(state, goal);
                if (room_cell != -1 && !move_makes_sense(state, goal, room_cell + 1)) {
                    // check if the way is free
                    int goal_h = 2 + 2 * goal;
                    if (hallway_free(state, h, goal_h)) {
                        int step_cost = energy[goal] * room_cell;
                        assert(room(state, goal)[room_cell] == '.');

                        hallway(state)[h] = '.';
                        room(state, goal)[room_cell] = goal + 'A';
                        if (!map_get(&settled, state)) {
                            heap_insert_or_decrease(&frontier, state, cost + step_cost + estimate_cost(state));
                        }
                        hallway(state)[h] = goal + 'A';
                        room(state, goal)[room_cell] = '.';
                    }
                }
            }
        }

        // move from a room into a hallway
        for (int r = 0; r < ROOM_COUNT; ++r) {
            // check if there is someting to move
            int room_cell = find_move_cell(state, r);
            if (room_cell != -1 && move_makes_sense(state, r, room_cell)) {
                for (int h = 0; h < HALLWAY_SIZE; ++h) {
                    // check if the destination is valid and free
                    if (h != 2 && h != 4 && h != 6 && h != 8 && hallway(state)[h] == '.') {
                        // check if the way is free
                        int source_h = 2 + 2 * r;
                        if (hallway_free(state, source_h, h)) {
                            int amphipod = room(state, r)[room_cell] - 'A';
                            assert(0 <= amphipod && amphipod < 4);
                            int goal_h = 2 + 2 * amphipod;
                            int step_cost = energy[amphipod] * (abs(source_h - h) + (room_cell + 2) + abs(h - goal_h));

                            hallway(state)[h] = amphipod + 'A';
                            room(state, r)[room_cell] = '.';
                            if (!map_get(&settled, state)) {
                                heap_insert_or_decrease(&frontier, state, cost + step_cost + estimate_cost(state));
                            }
                            hallway(state)[h] = '.';
                            room(state, r)[room_cell] = amphipod + 'A';
                        }
                    }
                }
            }
        }
    }

    assert(false);
}

int main() {
    state *initial_state = malloc(sizeof(state) + HALLWAY_SIZE + ROOM_COUNT * 2);
    initial_state->room_size = 2;
    memset(initial_state->state, '.', HALLWAY_SIZE + ROOM_COUNT * 2);

    int result = scanf("%*s\n");
    assert(result == 0);
    result = scanf("%*s\n");
    assert(result == 0);
    result = scanf("###%c#%c#%c#%c###\n", &room(initial_state, 0)[0], &room(initial_state, 1)[0],
                   &room(initial_state, 2)[0], &room(initial_state, 3)[0]);
    assert(result == 4);
    result = scanf("  #%c#%c#%c#%c#  \n", &room(initial_state, 0)[1], &room(initial_state, 1)[1],
                   &room(initial_state, 2)[1], &room(initial_state, 3)[1]);
    assert(result == 4);

    state *goal_state = malloc(sizeof(state) + HALLWAY_SIZE + ROOM_COUNT * 2);
    goal_state->room_size = 2;
    memcpy(goal_state->state, "...........AABBCCDD", HALLWAY_SIZE + ROOM_COUNT * 2);
    printf("%d\n", search(initial_state, goal_state));

    state *initial_state2 = malloc(sizeof(state) + HALLWAY_SIZE + ROOM_COUNT * 4);
    initial_state2->room_size = 4;
    memset(initial_state2->state, '.', HALLWAY_SIZE + ROOM_COUNT * 2);
    for (int r = 0; r < ROOM_COUNT; ++r) {
        room(initial_state2, r)[0] = room(initial_state, r)[0];
        room(initial_state2, r)[3] = room(initial_state, r)[1];
    }
    room(initial_state2, 0)[1] = 'D';
    room(initial_state2, 1)[1] = 'C';
    room(initial_state2, 2)[1] = 'B';
    room(initial_state2, 3)[1] = 'A';
    room(initial_state2, 0)[2] = 'D';
    room(initial_state2, 1)[2] = 'B';
    room(initial_state2, 2)[2] = 'A';
    room(initial_state2, 3)[2] = 'C';

    state *goal_state2 = malloc(sizeof(state) + HALLWAY_SIZE + ROOM_COUNT * 4);
    goal_state2->room_size = 4;
    memcpy(goal_state2->state, "...........AAAABBBBCCCCDDDD", HALLWAY_SIZE + ROOM_COUNT * 4);
    printf("%d\n", search(initial_state2, goal_state2));

    free(goal_state);
    free(goal_state2);
    free(initial_state);
    free(initial_state2);
    return 0;
}
