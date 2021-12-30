#include "arraylist.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static char *get(arraylist *grid, int x, int y, int w) { return arraylist_get(grid, y * w + x); }

int main() {
    arraylist grid;
    arraylist_init(&grid, sizeof(char));
    int x = 0, y = 0, w = 0;
    for (;;) {
        int c = fgetc(stdin);
        if (c == EOF) {
            assert(x == 0);
            assert(w != 0);
            break;
        } else if (c == '\n') {
            if (w == 0) {
                w = x;
            }
            assert(x == w);
            y += 1;
            x = 0;
        } else {
            assert(c == '.' || c == '>' || c == 'v');
            arraylist_append(&grid, &c);
            x += 1;
        }
    }
    int h = y;

    arraylist grid2;
    arraylist_init(&grid2, sizeof(char));
    for (size_t i = 0; i < arraylist_size(&grid); ++i) {
        const char dot = '.';
        arraylist_append(&grid2, &dot);
    }

    arraylist *current = &grid, *next = &grid2;
    int iter = 1;
    for (;; ++iter) {
        for (char *n = arraylist_begin(next), *c = arraylist_begin(current); n != arraylist_end(next); ++n, ++c) {
            *n = *c;
        }

        bool did_move = false;

        for (int x = 0; x < w; ++x) {
            for (int y = 0; y < h; ++y) {
                if (*get(current, x, y, w) == '>') {
                    if (*get(current, (x + 1) % w, y, w) == '.') {
                        did_move = true;
                        *get(next, (x + 1) % w, y, w) = '>';
                        *get(next, x, y, w) = '.';
                    } else {
                        // nothing to do
                    }
                }
            }
        }

        for (char *n = arraylist_begin(next), *c = arraylist_begin(current); n != arraylist_end(next); ++n, ++c) {
            *c = *n;
        }

        for (int x = 0; x < w; ++x) {
            for (int y = 0; y < h; ++y) {
                if (*get(next, x, y, w) == 'v') {
                    if (*get(next, x, (y + 1) % h, w) == '.') {
                        did_move = true;
                        *get(current, x, (y + 1) % h, w) = 'v';
                        *get(current, x, y, w) = '.';
                    } else {
                        // nothing to do
                    }
                }
            }
        }

        if (!did_move) {
            break;
        }
    }

    printf("%d\n", iter);

    arraylist_free(&grid2);
    arraylist_free(&grid);
    return 0;
}
