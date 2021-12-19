#include "arraylist.h"
#include "map.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct vec3 {
    int x;
    int y;
    int z;
} vec3;

static vec3 vec3_add(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }

static bool vec3_eq(vec3 a, vec3 b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

static bool vec3_eq_ptr(const void *a, const void *b) { return vec3_eq(*(const vec3 *)a, *(const vec3 *)b); }

static int vec3_manhattan_dist(vec3 a, vec3 b) { return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z); }

static size_t vec3_hash(const void *vp) {
    vec3 v = *(const vec3 *)vp;
    return (size_t)v.x * 1234 + (size_t)v.y * 66454 + (size_t)v.z * 453464;
}

typedef vec3 (*orientation)(vec3 p);

typedef struct scanner {
    arraylist points;
    bool done;
    vec3 position;
    orientation orientation;
} scanner;

static void scanner_init(scanner *scanner) {
    arraylist_init(&scanner->points, sizeof(vec3));
    scanner->done = false;
}

static void scanner_free(scanner *scanner) { arraylist_free(&scanner->points); }

#define ORIENTATION(NAME, X, Y, Z)                                                                                     \
    static vec3 look_##NAME##_a(vec3 p) { return (vec3){+(X), +(Y), +(Z)}; }                                           \
    static vec3 look_##NAME##_b(vec3 p) { return (vec3){+(X), +(Z), -(Y)}; }                                           \
    static vec3 look_##NAME##_c(vec3 p) { return (vec3){+(X), -(Y), -(Z)}; }                                           \
    static vec3 look_##NAME##_d(vec3 p) { return (vec3){+(X), -(Z), +(Y)}; }
ORIENTATION(posx, +p.x, +p.y, +p.z)
ORIENTATION(negx, -p.x, +p.y, -p.z)
ORIENTATION(posy, +p.y, -p.x, +p.z)
ORIENTATION(negy, -p.y, +p.x, +p.z)
ORIENTATION(posz, +p.z, +p.y, -p.x)
ORIENTATION(negz, -p.z, +p.y, +p.x)
static const orientation orientations[] = {
    look_posx_a, look_posx_b, look_posx_c, look_posx_d, look_negx_a, look_negx_b, look_negx_c, look_negx_d,
    look_posy_a, look_posy_b, look_posy_c, look_posy_d, look_negy_a, look_negy_b, look_negy_c, look_negy_d,
    look_posz_a, look_posz_b, look_posz_c, look_posz_d, look_negz_a, look_negz_b, look_negz_c, look_negz_d,
};

static bool all_done(arraylist *scanners) {
    for (scanner *scanner = arraylist_begin(scanners); scanner != arraylist_end(scanners); ++scanner) {
        if (!scanner->done) {
            return false;
        }
    }
    return true;
}

static bool enough_points_match(scanner *a, scanner *b) {
    int count = 0;
    for (vec3 *ap = arraylist_begin(&a->points); ap != arraylist_end(&a->points); ++ap) {
        vec3 global_ap = vec3_add(a->orientation(*ap), a->position);
        for (vec3 *bp = arraylist_begin(&b->points); bp != arraylist_end(&b->points); ++bp) {
            vec3 global_bp = vec3_add(b->orientation(*bp), b->position);
            if (vec3_eq(global_ap, global_bp)) {
                count += 1;
                if (count == 12) {
                    return true;
                }
            }
        }
    }
    return false;
}

static bool try_match(scanner *done, scanner *todo) {
    assert(done->done);
    for (size_t oi = 0; oi < sizeof(orientations) / sizeof(orientations[0]); ++oi) {
        orientation orientation = orientations[oi];
        for (vec3 *dp = arraylist_begin(&done->points); dp != arraylist_end(&done->points); ++dp) {
            vec3 global_dp = vec3_add(done->orientation(*dp), done->position);
            for (vec3 *tp = arraylist_begin(&todo->points); tp != arraylist_end(&todo->points); ++tp) {
                vec3 otp = orientation(*tp);
                vec3 position = (vec3){global_dp.x - otp.x, global_dp.y - otp.y, global_dp.z - otp.z};
                todo->orientation = orientation;
                todo->position = position;
                if (enough_points_match(done, todo)) {
                    fprintf(stderr, "found scanner at %d,%d,%d (rot %zd)\n", position.x, position.y, position.z, oi);
                    todo->done = true;
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    arraylist scanners;
    arraylist_init(&scanners, sizeof(scanner));
    for (;;) {
        int c1 = fgetc(stdin);
        if (c1 == EOF) {
            break;
        }
        int c2 = fgetc(stdin);
        assert(c2 != EOF);
        if (c1 == '-' && c2 == '-') {
            int result = scanf("%*[^\n]\n");
            assert(result == 0);
            scanner scanner;
            scanner_init(&scanner);
            arraylist_append(&scanners, &scanner);
        } else {
            int result = ungetc(c2, stdin);
            assert(result == c2);
            result = ungetc(c1, stdin);
            assert(result == c1);
            vec3 p;
            result = scanf("%d,%d,%d\n", &p.x, &p.y, &p.z);
            assert(result == 3);
            arraylist_append(&((scanner *)arraylist_get(&scanners, arraylist_size(&scanners) - 1))->points, &p);
        }
    }
    assert(arraylist_size(&scanners) > 0);

    scanner *first = arraylist_get(&scanners, 0);
    first->done = true;
    first->orientation = orientations[0];
    first->position = (vec3){0, 0, 0};
    while (!all_done(&scanners)) {
        for (scanner *todo = arraylist_begin(&scanners); todo != arraylist_end(&scanners); ++todo) {
            if (todo->done) {
                continue;
            }
            for (scanner *done = arraylist_begin(&scanners); done != arraylist_end(&scanners); ++done) {
                if (!done->done) {
                    continue;
                }
                if (try_match(done, todo)) {
                    break;
                }
            }
        }
    }

    map points;
    map_init(&points, sizeof(vec3), vec3_hash, vec3_eq_ptr, 0);
    for (scanner *scanner = arraylist_begin(&scanners); scanner != arraylist_end(&scanners); ++scanner) {
        for (vec3 *p = arraylist_begin(&scanner->points); p != arraylist_end(&scanner->points); ++p) {
            vec3 global_p = vec3_add(scanner->orientation(*p), scanner->position);
            map_put(&points, &global_p, NULL);
        }
    }
    printf("%zd\n", map_size(&points));

    int max_dist = INT_MIN;
    for (scanner *a = arraylist_begin(&scanners); a != arraylist_end(&scanners); ++a) {
        for (scanner *b = arraylist_begin(&scanners); b != arraylist_end(&scanners); ++b) {
            int dist = vec3_manhattan_dist(a->position, b->position);
            if (dist > max_dist) {
                max_dist = dist;
            }
        }
    }
    printf("%d\n", max_dist);

    map_free(&points);
    for (scanner *scanner = arraylist_begin(&scanners); scanner != arraylist_end(&scanners); ++scanner) {
        scanner_free(scanner);
    }
    arraylist_free(&scanners);
    return 0;
}
