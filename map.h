/**
 * A simple hash map with linear probing.
 * The keys and values must be copyable with memcpy.
 * Both are not free'd by the container.
 */

#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef enum map_state { MAP_EMPTY, MAP_ACTIVE, MAP_SKIP } map_state;

typedef struct map {
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t (*hash_key)(const void *);
    bool (*equal_key)(const void *, const void *);
    size_t value_size;
    map_state *actives;
    char *keys;
    char *values;
} map;

typedef size_t map_iter;

void map_init(map *map, size_t key_size, size_t (*hash_key)(const void *),
              bool (*equal_key)(const void *, const void *), size_t value_size);
void map_free(map *map);

void *map_put(map *map, const void *key, const void *value);
map_iter map_remove(map *map, const void *key);

void *map_get(const map *map, const void *key);
map_iter map_begin(const map *map);
map_iter map_advance(const map *map, map_iter iter);
map_iter map_end(const map *map);
void *map_key(const map *map, map_iter iter);
void *map_value(const map *map, map_iter iter);
size_t map_size(const map *map);
