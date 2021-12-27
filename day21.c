#include "map.h"
#include <assert.h>
#include <stdio.h>

static long zero = 0;
static long one = 1;

static int d100(int *rolls) { return (*rolls)++ % 100 + 1; }

typedef struct state {
    int pos_a;
    int pos_b;
    int score_a;
    int score_b;
} state;

static bool state_eq(const void *a, const void *b) {
    const state *aa = a;
    const state *bb = b;
    return aa->pos_a == bb->pos_a && aa->pos_b == bb->pos_b && aa->score_a == bb->score_a && aa->score_b == bb->score_b;
}

static size_t state_hash(const void *a) {
    const state *aa = a;
    return (size_t)aa->pos_a * 2342 + (size_t)aa->pos_b * 4564 + (size_t)aa->score_a * 35435 +
           (size_t)aa->score_b * 65656;
}

static void move_quantum(state current, long count, map *dest) {
    for (int a = 1; a <= 3; ++a) {
        for (int b = 1; b <= 3; ++b) {
            for (int c = 1; c <= 3; ++c) {
                state next = current;
                next.pos_a = (next.pos_a + a + b + c) % 10;
                next.score_a += next.pos_a + 1;
                if (next.score_a >= 21) {
                    long *next_cnt = map_get(dest, &next);
                    if (!next_cnt) {
                        next_cnt = map_put(dest, &next, &zero);
                    }
                    *next_cnt += count;
                } else {
                    for (int d = 1; d <= 3; ++d) {
                        for (int e = 1; e <= 3; ++e) {
                            for (int f = 1; f <= 3; ++f) {
                                state inner_next = next;
                                inner_next.pos_b = (inner_next.pos_b + d + e + f) % 10;
                                inner_next.score_b += inner_next.pos_b + 1;
                                long *next_cnt = map_get(dest, &inner_next);
                                if (!next_cnt) {
                                    next_cnt = map_put(dest, &inner_next, &zero);
                                }
                                *next_cnt += count;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    int player_a, player_b;
    int result = scanf("Player 1 starting position: %d\n", &player_a);
    assert(result == 1);
    result = scanf("Player 2 starting position: %d\n", &player_b);
    assert(result == 1);
    player_a -= 1;
    player_b -= 1;

    int rolls = 0;
    int score_a = 0;
    int score_b = 0;
    int pos_a = player_a;
    int pos_b = player_b;
    for (;;) {
        pos_a = (pos_a + d100(&rolls) + d100(&rolls) + d100(&rolls)) % 10;
        score_a += pos_a + 1;
        if (score_a >= 1000) {
            break;
        }
        pos_b = (pos_b + d100(&rolls) + d100(&rolls) + d100(&rolls)) % 10;
        score_b += pos_b + 1;
        if (score_b >= 1000) {
            break;
        }
    }
    printf("%d\n", (score_a >= 1000) ? (rolls * score_b) : (rolls * score_a));

    map states, states2;
    map_init(&states, sizeof(state), state_hash, state_eq, sizeof(long));
    map_init(&states2, sizeof(state), state_hash, state_eq, sizeof(long));
    map *current = &states;
    map *next = &states2;

    state initial_state = {.pos_a = player_a, .pos_b = player_b, .score_a = 0, .score_b = 0};
    map_put(current, &initial_state, &one);
    long won_a = 0;
    long won_b = 0;
    while (map_size(current)) {
        map_clear(next);

        for (map_iter i = map_begin(current); i != map_end(current); i = map_advance(current, i)) {
            state state = *(struct state *)map_key(current, i);
            long count = *(long *)map_value(current, i);
            move_quantum(state, count, next);
        }

        for (map_iter i = map_begin(next); i != map_end(next); i = map_advance(next, i)) {
            state state = *(struct state *)map_key(next, i);
            long count = *(long *)map_value(next, i);
            if (state.score_a >= 21) {
                assert(state.score_b < 21);
                won_a += count;
                map_remove(next, &state);
            } else if (state.score_b >= 21) {
                won_b += count;
                map_remove(next, &state);
            }
        }

        map *tmp = current;
        current = next;
        next = tmp;
    }
    printf("%ld\n", won_a > won_b ? won_a : won_b);

    map_free(&states);
    map_free(&states2);
    return 0;
}