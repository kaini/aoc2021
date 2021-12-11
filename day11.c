#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define SIZE 10
#define FLASH_ENERGY 10

typedef int grid[SIZE][SIZE];

static int flash(grid *grid, int x, int y) {
    int flashes = 1;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0)
                continue;
            if (x + dx < 0)
                continue;
            if (x + dx >= SIZE)
                continue;
            if (y + dy < 0)
                continue;
            if (y + dy >= SIZE)
                continue;
            (*grid)[x + dx][y + dy] += 1;
            if ((*grid)[x + dx][y + dy] == FLASH_ENERGY) {
                flashes += flash(grid, x + dx, y + dy);
            }
        }
    }
    return flashes;
}

int main() {
    grid grid;
    int x = 0;
    int y = 0;
    for (;;) {
        int c = getc(stdin);
        if (c == EOF) {
            break;
        } else if (c == '\n') {
            assert(x == SIZE);
            assert(y < SIZE);
            x = 0;
            y += 1;
        } else if ('0' <= c && c <= '9') {
            assert(x < SIZE);
            grid[x][y] = c - '0';
            x += 1;
        } else {
            assert(0);
        }
    }
    assert(y == SIZE && x == 0);

    int flashes = 0;
    int all_flashed = -1;
    for (int step = 0; step < 100 || all_flashed == -1; ++step) {
        int step_flashes = 0;
        for (int x = 0; x < SIZE; ++x) {
            for (int y = 0; y < SIZE; ++y) {
                grid[x][y] += 1;
                if (grid[x][y] == FLASH_ENERGY) {
                    step_flashes += flash(&grid, x, y);
                }
            }
        }

        for (int x = 0; x < SIZE; ++x) {
            for (int y = 0; y < SIZE; ++y) {
                if (grid[x][y] >= FLASH_ENERGY) {
                    grid[x][y] = 0;
                }
            }
        }

        if (step < 100) {
            flashes += step_flashes;
        }
        if (step_flashes == SIZE * SIZE) {
            all_flashed = step;
        }
    }
    printf("%d\n%d\n", flashes, all_flashed + 1);

    return 0;
}