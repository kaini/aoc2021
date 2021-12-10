#include "arraylist.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>

typedef struct xy {
    int x;
    int y;
} xy;

static size_t hash_xy(const void *xy_ptr) {
    xy xy = *(const struct xy *)xy_ptr;
    return xy.x * 12314 + xy.y * 23112;
}

static bool eq_xy(const void *a, const void *b) {
    const xy *aa = a;
    const xy *bb = b;
    return aa->x == bb->x && aa->y == bb->y;
}

static int cmp_size_t(const void *a, const void *b) {
    int aa = *(const size_t *)a;
    int bb = *(const size_t *)b;
    if (aa > bb) {
        return -1;
    } else if (aa < bb) {
        return 1;
    } else {
        return 0;
    }
}

static void floodfill(map *grid, map *filled, xy point) {
    if (map_get(filled, &point)) {
        return;
    }

    int *height = map_get(grid, &point);
    if (!height || *height == 9) {
        return;
    }

    map_put(filled, &point, NULL);
    floodfill(grid, filled, (xy){point.x - 1, point.y});
    floodfill(grid, filled, (xy){point.x + 1, point.y});
    floodfill(grid, filled, (xy){point.x, point.y - 1});
    floodfill(grid, filled, (xy){point.x, point.y + 1});
}

int main() {
    map grid;
    map_init(&grid, sizeof(xy), hash_xy, eq_xy, sizeof(int));
    int x = 0;
    int height = 0;
    int width = -1;
    for (;;) {
        int c = getc(stdin);
        if (c == EOF || c == '\n') {
            if (width == -1) {
                width = x;
            }
            if (x > 0) {
                height += 1;
                assert(x == width);
            }
            x = 0;
            if (c == EOF) {
                break;
            } else {
                continue;
            }
        } else if (c == '\r') {
            continue;
        }
        assert('0' <= c && c <= '9');
        int cell = c - '0';
        map_put(&grid, &(xy){x, height}, &cell);
        x += 1;
    }

    int risk_level = 0;
    arraylist lowpoints;
    arraylist_init(&lowpoints, sizeof(xy));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            int *self = map_get(&grid, &(xy){x, y});
            int *left = map_get(&grid, &(xy){x - 1, y});
            if (left && *left <= *self) {
                continue;
            }
            int *right = map_get(&grid, &(xy){x + 1, y});
            if (right && *right <= *self) {
                continue;
            }
            int *top = map_get(&grid, &(xy){x, y - 1});
            if (top && *top <= *self) {
                continue;
            }
            int *bottom = map_get(&grid, &(xy){x, y + 1});
            if (bottom && *bottom <= *self) {
                continue;
            }
            risk_level += *self + 1;
            arraylist_append(&lowpoints, &(xy){x, y});
        }
    }
    printf("%d\n", risk_level);

    arraylist sizes;
    arraylist_init(&sizes, sizeof(size_t));
    for (xy *lowpoint = arraylist_begin(&lowpoints); lowpoint != arraylist_end(&lowpoints); ++lowpoint) {
        map filled;
        map_init(&filled, sizeof(xy), hash_xy, eq_xy, 0);
        floodfill(&grid, &filled, *lowpoint);
        size_t filled_size = map_size(&filled);
        arraylist_append(&sizes, &filled_size);
        map_free(&filled);
    }
    arraylist_sort(&sizes, cmp_size_t);
    assert(arraylist_size(&sizes) >= 3);
    printf("%zu\n", *(size_t *)arraylist_get(&sizes, 0) * *(size_t *)arraylist_get(&sizes, 1) *
                        *(size_t *)arraylist_get(&sizes, 2));

    arraylist_free(&sizes);
    arraylist_free(&lowpoints);
    map_free(&grid);
    return 0;
}
