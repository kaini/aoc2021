#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct map {
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t (*hash_key)(const void*);
    bool (*equal_key)(const void*, const void*);
    void (*free_key)(void*);
    size_t value_size;
    void (*free_value)(void*);
    bool* actives;
    char* keys;
    char* values;
} map;

typedef size_t map_iter;

void map_init(map* map, size_t key_size, size_t (*hash_key)(const void*), bool (*equal_key)(const void*, const void*), void (*free_key)(void*), size_t value_size, void (*free_value)(void*));
void map_free(map* map);

void map_put(map* map, const void* key, const void* value);

void* map_get(const map* map, const void* key);
map_iter map_begin(const map* map);
map_iter map_advance(const map* map, map_iter iter);
map_iter map_end(const map* map);
void* map_key(const map* map, map_iter iter);
void* map_value(const map* map, map_iter iter);
