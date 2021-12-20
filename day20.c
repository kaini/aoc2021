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

static const xy infinity = {INT_MAX, INT_MAX};
static const bool btrue = true;
static const bool bfalse = false;

static size_t hash_xy(const void *xy_ptr) {
    xy xy = *(const struct xy *)xy_ptr;
    return (size_t)xy.x * 12314 + (size_t)xy.y * 23112;
}

static bool eq_xy(const void *a, const void *b) {
    const xy *aa = a;
    const xy *bb = b;
    return aa->x == bb->x && aa->y == bb->y;
}

static bool lookup(map *image, int x, int y) {
    bool *value = map_get(image, &(xy){x, y});
    if (value) {
        return *value;
    } else {
        return *(bool *)map_get(image, &infinity);
    }
}

static void apply(arraylist *mapping, map *source, map *dest) {
    if (*(bool *)map_get(source, &infinity)) {
        map_put(dest, &infinity, arraylist_get(mapping, 511));
    } else {
        map_put(dest, &infinity, arraylist_get(mapping, 0));
    }

    int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
    for (map_iter i = map_begin(source); i != map_end(source); i = map_advance(source, i)) {
        xy p = *(xy *)map_key(source, i);
        if (p.x == infinity.x && p.y == infinity.y)
            continue;
        if (p.x < minx)
            minx = p.x;
        if (p.x > maxx)
            maxx = p.x;
        if (p.y < miny)
            miny = p.y;
        if (p.y > maxy)
            maxy = p.y;
    }
    minx -= 1;
    miny -= 1;
    maxx += 1;
    maxy += 1;

    for (int x = minx; x <= maxx; ++x) {
        for (int y = miny; y <= maxy; ++y) {
            int index = (lookup(source, x - 1, y - 1) << 8) | //
                        (lookup(source, x + 0, y - 1) << 7) | //
                        (lookup(source, x + 1, y - 1) << 6) | //
                        (lookup(source, x - 1, y + 0) << 5) | //
                        (lookup(source, x + 0, y + 0) << 4) | //
                        (lookup(source, x + 1, y + 0) << 3) | //
                        (lookup(source, x - 1, y + 1) << 2) | //
                        (lookup(source, x + 0, y + 1) << 1) | //
                        (lookup(source, x + 1, y + 1) << 0);  //
            map_put(dest, &(xy){x, y}, arraylist_get(mapping, index));
        }
    }
}

static int count_lit(map *image) {
    int count = 0;
    for (map_iter i = map_begin(image); i != map_end(image); i = map_advance(image, i)) {
        count += *(bool *)map_value(image, i);
    }
    return count;
}

int main() {
    arraylist mapping;
    arraylist_init(&mapping, sizeof(bool));
    for (;;) {
        char c = getc(stdin);
        if (c == '.') {
            arraylist_append(&mapping, &bfalse);
        } else if (c == '#') {
            arraylist_append(&mapping, &btrue);
        } else if (c == '\n') {
            c = getc(stdin);
            assert(c == '\n');
            break;
        } else {
            assert(false);
        }
    }
    assert(arraylist_size(&mapping) == 512);

    map image;
    map_init(&image, sizeof(xy), hash_xy, eq_xy, sizeof(bool));
    int x = 0;
    int y = 0;
    int width = -1;
    for (;;) {
        char c = getc(stdin);
        if (c == EOF) {
            break;
        } else if (c == '\n') {
            if (width == -1) {
                width = x;
            }
            assert(x == width);
            x = 0;
            y += 1;
        } else if (c == '.') {
            bool b = false;
            map_put(&image, &(xy){x, y}, &b);
            x += 1;
        } else if (c == '#') {
            bool b = true;
            map_put(&image, &(xy){x, y}, &b);
            x += 1;
        } else {
            assert(false);
        }
    }
    map_put(&image, &infinity, &bfalse);

    map image2;
    map_init(&image2, sizeof(xy), hash_xy, eq_xy, sizeof(bool));

    map *a = &image;
    map *b = &image2;
    for (int i = 0; i < 50; ++i) {
        map_clear(b);
        apply(&mapping, a, b);
        map *tmp = a;
        a = b;
        b = tmp;

        if (i == 1 || i == 49) {
            printf("%d\n", count_lit(a));
        }
    }

    map_free(&image2);
    map_free(&image);
    arraylist_free(&mapping);
    return 0;
}
