#include "arraylist.h"
#include "map.h"
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

typedef struct line {
    int x1;
    int y1;
    int x2;
    int y2;
} line;

typedef struct xy {
    int x;
    int y;
} xy;

static size_t hash_xy(const void* value) {
    return ((xy*)value)->x * 3109 + ((xy*)value)->y * 269;
}

static bool equal_xy(const void* a, const void* b) {
    return ((xy*)a)->x == ((xy*)b)->x && ((xy*)a)->y == ((xy*)b)->y;
}

static bool is_diagonal(line line) {
    return line.x1 != line.x2 && line.y1 != line.y2;
}

static int count_cells_with_more_lines(map* line_count) {
    int result = 0;
    for (map_iter iter = map_begin(line_count); iter != map_end(line_count); iter = map_advance(line_count, iter)) {
        int* count = map_value(line_count, iter);
        if (*count >= 2) result += 1;
    }
    return result;
}

int main() {
    arraylist lines;
    arraylist_init(&lines, sizeof(line), NULL);
    for (;;) {
        line line;
        int result = scanf("%d,%d -> %d,%d", &line.x1, &line.y1, &line.x2, &line.y2);
        if (result == EOF) {
            break;
        }
        assert(result == 4);
        arraylist_append(&lines, &line);
    }

    map line_count;
    map_init(&line_count, sizeof(xy), hash_xy, equal_xy, NULL, sizeof(int), NULL);
    for (line* line = arraylist_begin(&lines); line != arraylist_end(&lines); ++line) {
        if (!is_diagonal(*line)) {
            if (line->x1 == line->x2) {
                int y_start = line->y1 < line->y2 ? line->y1 : line->y2;
                int y_end = line->y1 < line->y2 ? line->y2 : line->y1;
                for (int y = y_start; y <= y_end; ++y) {
                    xy key = { .x = line->x1, .y = y };
                    int* value_ptr = map_get(&line_count, &key);
                    int value = (value_ptr ? *value_ptr : 0) + 1;
                    map_put(&line_count, &key, &value);
                }
            } else {
                int x_start = line->x1 < line->x2 ? line->x1 : line->x2;
                int x_end = line->x1 < line->x2 ? line->x2 : line->x1;
                for (int x = x_start; x <= x_end; ++x) {
                    xy key = { .x = x, .y = line->y1 };
                    int* value_ptr = map_get(&line_count, &key);
                    int value = (value_ptr ? *value_ptr : 0) + 1;
                    map_put(&line_count, &key, &value);
                }
            }
        }
    }
    printf("%d\n", count_cells_with_more_lines(&line_count));

    for (line* line = arraylist_begin(&lines); line != arraylist_end(&lines); ++line) {
        if (is_diagonal(*line)) {
            int x = line->x1;
            int y = line->y1;
            for (;;) {
                xy key = { .x = x, .y = y };
                int* value_ptr = map_get(&line_count, &key);
                int value = (value_ptr ? *value_ptr : 0) + 1;
                map_put(&line_count, &key, &value);
                if (x == line->x2) {
                    assert(y == line->y2);
                    break;
                }
                if (line->x2 < x) x -= 1; else x += 1;
                if (line->y2 < y) y -= 1; else y += 1;
            }
        }
    }
    printf("%d\n", count_cells_with_more_lines(&line_count));
    
    map_free(&line_count);
    arraylist_free(&lines);
    return 0;
}
