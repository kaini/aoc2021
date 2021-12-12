#include "arraylist.h"
#include "map.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char cave_name[10];

static size_t cave_name_hash(const void *cave_name_v) {
    const char *cave_name = cave_name_v;
    size_t result = 0;
    int i = 0;
    while (*cave_name) {
        result += *cave_name * i * 2343;
        cave_name += 1;
        i += 1;
    }
    return result;
}

static bool cave_name_eq(const void *a, const void *b) { return strcmp(a, b) == 0; }

typedef struct cave {
    bool is_small;
    arraylist neighbours;
    int visited;
} cave;

static cave *alloc_cave(cave_name *name) {
    cave *cave = malloc(sizeof(*cave));
    cave->is_small = 'a' <= (*name)[0] && (*name)[0] <= 'z';
    arraylist_init(&cave->neighbours, sizeof(struct cave *));
    cave->visited = 0;
    return cave;
}

static void free_cave(cave *cave) {
    arraylist_free(&cave->neighbours);
    free(cave);
}

static int count_paths(cave *at, cave *start, cave *end, bool one_double) {
    if (at == end) {
        return 1;
    }

    int count = 0;
    at->visited += 1;
    for (cave **n = arraylist_begin(&at->neighbours); n != arraylist_end(&at->neighbours); ++n) {
        if (!(*n)->is_small || !(*n)->visited) {
            count += count_paths(*n, start, end, one_double);
        } else if (one_double && *n != start && *n != end) {
            count += count_paths(*n, start, end, false);
        }
    }
    at->visited -= 1;
    return count;
}

int main() {
    map caves;
    map_init(&caves, sizeof(cave_name), cave_name_hash, cave_name_eq, sizeof(cave *));
    cave *start = NULL;
    cave *end = NULL;
    for (;;) {
        cave_name name_a, name_b;
        int result = scanf("%9[a-zA-Z]-%9[a-zA-Z]\n", name_a, name_b);
        if (result == EOF) {
            break;
        }
        assert(result == 2);

        cave **a_ptr = map_get(&caves, &name_a);
        cave *a;
        if (a_ptr) {
            a = *a_ptr;
        } else {
            a = alloc_cave(&name_a);
            map_put(&caves, &name_a, &a);
        }
        cave **b_ptr = map_get(&caves, &name_b);
        cave *b;
        if (b_ptr) {
            b = *b_ptr;
        } else {
            b = alloc_cave(&name_b);
            map_put(&caves, &name_b, &b);
        }

        arraylist_append(&a->neighbours, &b);
        arraylist_append(&b->neighbours, &a);

        if (!start && strcmp(name_a, "start") == 0) {
            start = a;
        }
        if (!start && strcmp(name_b, "start") == 0) {
            start = b;
        }
        if (!end && strcmp(name_a, "end") == 0) {
            end = a;
        }
        if (!end && strcmp(name_b, "end") == 0) {
            end = b;
        }
    }
    assert(start);
    assert(end);

    printf("%d\n%d\n", count_paths(start, start, end, false), count_paths(start, start, end, true));

    for (map_iter cave = map_begin(&caves); cave != map_end(&caves); cave = map_advance(&caves, cave)) {
        free_cave(*(struct cave **)map_value(&caves, cave));
    }
    map_free(&caves);
    return 0;
}
