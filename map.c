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

static void *map_put_raw(map *map, const void *key, const void *value, bool update_size) {
    size_t seen_skip = (size_t)-1;
    size_t slot = map->hash_key(key) % map->capacity;
    while (map->actives[slot] != MAP_EMPTY) {
        if (seen_skip == (size_t)-1 && map->actives[slot] == MAP_SKIP) {
            seen_skip = slot;
        }
        if (map->actives[slot] == MAP_ACTIVE && map->equal_key(key, map->keys + map->key_size * slot)) {
            break;
        }
        slot = (slot + 1) % map->capacity;
    }
    if (seen_skip != (size_t)-1 && map->actives[slot] != MAP_ACTIVE) {
        slot = seen_skip;
    }

    if (update_size && map->actives[slot] != MAP_ACTIVE) {
        map->size += 1;
    }
    map->actives[slot] = MAP_ACTIVE;
    memcpy(map->keys + map->key_size * slot, key, map->key_size);
    memcpy(map->values + map->value_size * slot, value, map->value_size);
    return map->values + map->value_size * slot;
}

void *map_put(map *map, const void *key, const void *value) {
    assert(map);
    assert(key);

    // Keep a load factor of at most 50 %
    if (map->size * 2 >= map->capacity) {
        map_state *old_actives = map->actives;
        char *old_keys = map->keys;
        char *old_values = map->values;
        size_t old_capacity = map->capacity;

        map->capacity = (map->capacity + 1) * 2;
        map->actives = calloc(map->capacity, sizeof(map_state));
        map->keys = malloc(map->capacity * map->key_size);
        map->values = malloc(map->capacity * map->value_size);
        for (size_t i = 0; i < old_capacity; ++i) {
            if (old_actives[i] == MAP_ACTIVE) {
                map_put_raw(map, old_keys + map->key_size * i, old_values + map->value_size * i, false);
            }
        }

        free(old_actives);
        free(old_keys);
        free(old_values);
    }

    return map_put_raw(map, key, value, true);
}

map_iter map_remove(map *map, const void *key) {
    assert(map);
    assert(key);
    if (map->capacity == 0) {
        return map_end(map);
    }

    size_t slot = map->hash_key(key) % map->capacity;
    while (map->actives[slot] != MAP_EMPTY) {
        if (map->actives[slot] == MAP_ACTIVE && map->equal_key(key, map->keys + map->key_size * slot)) {
            map->actives[slot] = MAP_SKIP;
            map->size -= 1;
            return slot;
        }
        slot = (slot + 1) % map->capacity;
    }
    return map_end(map);
}

void map_clear(map *map) {
    assert(map);
    for (size_t i = 0; i < map->capacity; ++i) {
        map->actives[i] = MAP_EMPTY;
    }
    map->size = 0;
}

void *map_get(const map *map, const void *key) {
    assert(map);
    assert(key);
    if (map->capacity == 0) {
        return NULL;
    }

    size_t slot = map->hash_key(key) % map->capacity;
    while (map->actives[slot] != MAP_EMPTY) {
        if (map->actives[slot] == MAP_ACTIVE && map->equal_key(key, map->keys + map->key_size * slot)) {
            return map->values + map->value_size * slot;
        }
        slot = (slot + 1) % map->capacity;
    }
    return NULL;
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
    } while (iter < map->capacity && map->actives[iter] != MAP_ACTIVE);
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

size_t map_size(const map *map) {
    assert(map);
    return map->size;
}
