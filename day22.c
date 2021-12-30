#include "arraylist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int min(int a, int b) { return a < b ? a : b; }

static int max(int a, int b) { return a > b ? a : b; }

typedef struct point {
    union {
        struct {
            int x, y, z;
        };
        int arr[3];
    };
} point;

static point p(int x, int y, int z) { return (point){{{x, y, z}}}; }

static point papply(point a, point b, int (*fn)(int, int)) { return p(fn(a.x, b.x), fn(a.y, b.y), fn(a.z, b.z)); }

static point pmin(point a, point b) { return papply(a, b, min); }

static point pmax(point a, point b) { return papply(a, b, max); }

typedef struct box {
    point min, max;
} box;

static const box small_bounding_box = {{{{-50, -50, -50}}}, {{{50, 50, 50}}}};

typedef struct box_entry {
    box box;
    int sign;
} box_entry;

static bool overlaps(const box *a, const box *b, box *output) {
    box result = {
        .min = pmax(a->min, b->min),
        .max = pmin(a->max, b->max),
    };
    if (result.min.x <= result.max.x && result.min.y <= result.max.y && result.min.z <= result.max.z) {
        if (output) {
            *output = result;
        }
        return true;
    } else {
        return false;
    }
}

static long volume(const box *box) {
    return (box->max.x - box->min.x + 1L) * (box->max.y - box->min.y + 1L) * (box->max.z - box->min.z + 1L);
}

static long entries_volume(arraylist *entries) {
    long result = 0;
    for (box_entry *entry = arraylist_begin(entries); entry != arraylist_end(entries); ++entry) {
        result += entry->sign * volume(&entry->box);
    }
    return result;
}

static void insert_box(arraylist *entries, const box *to_insert, bool add) {
    for (size_t i = 0, start_size = arraylist_size(entries); i < start_size; ++i) {
        box_entry *entry = arraylist_get(entries, i);
        box overlap;
        if (overlaps(to_insert, &entry->box, &overlap)) {
            box_entry new_entry = {
                .box = overlap,
                .sign = -entry->sign,
            };
            arraylist_append(entries, &new_entry);
        }
    }

    if (add) {
        box_entry new_entry = {
            .box = *to_insert,
            .sign = 1,
        };
        arraylist_append(entries, &new_entry);
    }
}

int main() {
    arraylist entries;
    arraylist_init(&entries, sizeof(box_entry));

    bool first_after_init = true;
    for (;;) {
        box box;
        char mode[4];
        int result = scanf("%3s x=%d..%d,y=%d..%d,z=%d..%d\n", mode, &box.min.x, &box.max.x, &box.min.y, &box.max.y,
                           &box.min.z, &box.max.z);
        if (result == EOF) {
            break;
        }
        assert(result == 7);
        assert(box.min.x <= box.max.x);
        assert(box.min.y <= box.max.y);
        assert(box.min.z <= box.max.z);
        assert(strcmp(mode, "on") == 0 || strcmp(mode, "off") == 0);

        if (overlaps(&box, &small_bounding_box, NULL)) {
            insert_box(&entries, &box, strcmp(mode, "on") == 0);
        } else {
            if (first_after_init) {
                first_after_init = false;
                printf("%ld\n", entries_volume(&entries));
            }
            insert_box(&entries, &box, strcmp(mode, "on") == 0);
        }
    }
    printf("%ld\n", entries_volume(&entries));

    arraylist_free(&entries);
    return 0;
}
