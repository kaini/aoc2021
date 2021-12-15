#include "arraylist.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>

static const int not_reached = -1;
static const int zero = 0;

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

static void do_step(map *costs, map *way_costs, arraylist *changed, int x, int y, int base_cost) {
    int *dest_step_cost = map_get(costs, &(xy){x, y});
    int *dest_way_cost = map_get(way_costs, &(xy){x, y});
    if (dest_step_cost && dest_way_cost &&
        (*dest_way_cost == not_reached || base_cost + *dest_step_cost < *dest_way_cost)) {
        *dest_way_cost = base_cost + *dest_step_cost;
        arraylist_append(changed, &(xy){x, y});
    }
}

static int bellman_ford(map *costs, int dest_x, int dest_y) {
    map way_costs;
    map_init(&way_costs, sizeof(xy), hash_xy, eq_xy, sizeof(int));
    for (map_iter iter = map_begin(costs); iter != map_end(costs); iter = map_advance(costs, iter)) {
        map_put(&way_costs, map_key(costs, iter), &not_reached);
    }

    arraylist updated1, updated2;
    arraylist_init(&updated1, sizeof(xy));
    arraylist_init(&updated2, sizeof(xy));
    arraylist *updated = &updated1;
    arraylist *next_updated = &updated2;

    map_put(&way_costs, &(xy){0, 0}, &zero);
    arraylist_append(updated, &(xy){0, 0});
    while (arraylist_size(updated)) {
        arraylist_clear(next_updated);

        for (xy *changed = arraylist_begin(updated); changed != arraylist_end(updated); ++changed) {
            int base_cost = *(int *)map_get(&way_costs, &(xy){changed->x, changed->y});
            assert(base_cost >= 0);
            do_step(costs, &way_costs, next_updated, changed->x - 1, changed->y, base_cost);
            do_step(costs, &way_costs, next_updated, changed->x + 1, changed->y, base_cost);
            do_step(costs, &way_costs, next_updated, changed->x, changed->y - 1, base_cost);
            do_step(costs, &way_costs, next_updated, changed->x, changed->y + 1, base_cost);
        }

        arraylist *temp = updated;
        updated = next_updated;
        next_updated = temp;
    }

    int result = *(int *)map_get(&way_costs, &(xy){dest_x, dest_y});
    map_free(&way_costs);
    arraylist_free(&updated1);
    arraylist_free(&updated2);
    return result;
}

int main() {
    map small_map;
    map_init(&small_map, sizeof(xy), hash_xy, eq_xy, sizeof(int));
    int x = 0;
    int y = 0;
    int width = 0;
    for (;;) {
        int c = getc(stdin);
        if (c == EOF) {
            break;
        } else if (c == '\n') {
            y += 1;
            if (width == 0) {
                width = x;
            }
            assert(x == width);
            x = 0;
        } else {
            assert('0' <= c && c <= '9');
            int value = c - '0';
            map_put(&small_map, &(xy){x, y}, &value);
            x += 1;
        }
    }
    int height = y;
    assert(width > 0 && height > 0 && width == height);

    printf("%d\n", bellman_ford(&small_map, width - 1, height - 1));

    map big_map;
    map_init(&big_map, sizeof(xy), hash_xy, eq_xy, sizeof(int));
    for (int x = 0; x < width * 5; ++x) {
        for (int y = 0; y < height * 5; ++y) {
            int cost = *(int *)map_get(&small_map, &(xy){x % width, y % height});
            cost += x / width;
            cost += y / width;
            if (cost > 9) {
                cost -= 9;
            }
            map_put(&big_map, &(xy){x, y}, &cost);
        }
    }
    printf("%d\n", bellman_ford(&big_map, width * 5 - 1, height * 5 - 1));

    map_free(&big_map);
    map_free(&small_map);
    return 0;
}
