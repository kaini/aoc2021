#include "arraylist.h"
#include "map.h"
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
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

typedef struct fold {
    bool along_x;
    int pos;
} fold;

static void do_fold(map *grid, map *new_grid, fold fold) {
    for (map_iter iter = map_begin(grid); iter != map_end(grid); iter = map_advance(grid, iter)) {
        xy p = *(xy *)map_key(grid, iter);
        if (fold.along_x) {
            if (p.x > fold.pos) {
                p.x = fold.pos - (p.x - fold.pos);
            }
        } else {
            if (p.y > fold.pos) {
                p.y = fold.pos - (p.y - fold.pos);
            }
        }
        map_put(new_grid, &p, NULL);
    }
}

int main() {
    map grid;
    map_init(&grid, sizeof(xy), hash_xy, eq_xy, 0);
    for (;;) {
        int c = getc(stdin);
        if (c == EOF) {
            break;
        }
        int result = ungetc(c, stdin);
        assert(result == c);
        if (c == 'f') {
            break;
        }

        int x, y;
        result = scanf("%d,%d\n", &x, &y);
        assert(result == 2);
        map_put(&grid, &(xy){x, y}, NULL);
    }

    arraylist folds;
    arraylist_init(&folds, sizeof(fold));
    for (;;) {
        char axis;
        int position;
        int result = scanf("fold along %c=%d\n", &axis, &position);
        if (result == EOF) {
            break;
        }
        assert(result == 2);
        arraylist_append(&folds, &(fold){.along_x = axis == 'x', .pos = position});
    }

    map new_grid;
    map_init(&new_grid, sizeof(xy), hash_xy, eq_xy, 0);
    assert(arraylist_size(&folds) > 0);
    do_fold(&grid, &new_grid, *(fold *)arraylist_get(&folds, 0));
    printf("%zd\n", map_size(&new_grid));

    map *grid_a = &new_grid;
    map *grid_b = &grid;
    for (fold *fold = ((struct fold *)arraylist_begin(&folds)) + 1; fold != arraylist_end(&folds); ++fold) {
        map_clear(grid_b);
        do_fold(grid_a, grid_b, *fold);
        map *grid_tmp = grid_a;
        grid_a = grid_b;
        grid_b = grid_tmp;
    }
    int min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;
    for (map_iter iter = map_begin(grid_a); iter != map_end(grid_a); iter = map_advance(grid_a, iter)) {
        xy p = *(xy *)map_key(grid_a, iter);
        if (p.x < min_x)
            min_x = p.x;
        if (p.x > max_x)
            max_x = p.x;
        if (p.y < min_y)
            min_y = p.y;
        if (p.y > max_y)
            max_y = p.y;
    }
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            if (map_get(grid_a, &(xy){x, y})) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    map_free(&new_grid);
    arraylist_free(&folds);
    map_free(&grid);
    return 0;
}