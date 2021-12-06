#include "map.h"
#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void map_init(map *map, size_t key_size, size_t (*hash_key)(const void *),
              bool (*equal_key)(const void *, const void *), size_t value_size) {
    assert(map);
    assert(key_size > 0);
    assert(hash_key);
    assert(equal_key);
    map->size = 0;
    map->capacity = 0;
    map->key_size = key_size;
    map->hash_key = hash_key;
    map->equal_key = equal_key;
    map->value_size = value_size;
    map->actives = NULL;
    map->keys = NULL;
    map->values = NULL;
}

void map_free(map *map) {
    free(map->actives);
    free(map->keys);
    free(map->values);
}

static void *map_put_raw(map *map, const void *key, const void *value) {
    size_t slot = map->hash_key(key) % map->capacity;
    while (map->actives[slot] && !map->equal_key(key, map->keys + map->key_size * slot)) {
        slot = (slot + 1) % map->capacity;
    }
    map->size += 1;
    map->actives[slot] = true;
    memcpy(map->keys + map->key_size * slot, key, map->key_size);
    memcpy(map->values + map->value_size * slot, value, map->value_size);
    return map->values + map->value_size * slot;
}

void *map_put(map *map, const void *key, const void *value) {
    assert(map);
    assert(key);

    // Keep a load factor of at most 50 %
    if (map->size * 2 >= map->capacity) {
        bool *old_actives = map->actives;
        char *old_keys = map->keys;
        char *old_values = map->values;
        size_t old_capacity = map->capacity;

        map->capacity = (map->capacity + 1) * 2;
        map->actives = calloc(map->capacity, sizeof(bool));
        map->keys = malloc(map->capacity * map->key_size);
        map->values = malloc(map->capacity * map->value_size);
        for (size_t i = 0; i < old_capacity; ++i) {
            if (old_actives[i]) {
                map_put_raw(map, old_keys + map->key_size * i, old_values + map->value_size * i);
            }
        }

        free(old_actives);
        free(old_keys);
        free(old_values);
    }

    return map_put_raw(map, key, value);
}

void *map_get(const map *map, const void *key) {
    assert(map);
    assert(key);
    if (map->capacity == 0) {
        return NULL;
    }

    size_t slot = map->hash_key(key) % map->capacity;
    while (map->actives[slot] && !map->equal_key(key, map->keys + map->key_size * slot)) {
        slot = (slot + 1) % map->capacity;
    }
    if (map->actives[slot]) {
        return map->values + map->value_size * slot;
    } else {
        return NULL;
    }
}

map_iter map_begin(const map *map) {
    assert(map);
    return map_advance(map, -1);
}

map_iter map_advance(const map *map, map_iter iter) {
    assert(map);
    assert(iter == (size_t)-1 || iter < map->capacity);
    do {
        iter += 1;
    } while (iter < map->capacity && !map->actives[iter]);
    return iter;
}

map_iter map_end(const map *map) {
    assert(map);
    return map->capacity;
}

void *map_key(const map *map, map_iter iter) {
    assert(map);
    assert(iter < map->capacity);
    return map->keys + map->key_size * iter;
}

void *map_value(const map *map, map_iter iter) {
    assert(map);
    assert(iter < map->capacity);
    return map->values + map->value_size * iter;
}
