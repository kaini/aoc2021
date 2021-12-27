#pragma once
#include "map.h"

typedef struct heap {
    size_t element_size;
    size_t capacity;
    map indices;
    char *buffer;
    int *prios;
} heap;

void heap_init(heap *heap, size_t element_size, size_t (*hash_element)(const void *),
               bool (*equal_element)(const void *, const void *));
void heap_free(heap *heap);

void heap_insert_or_decrease(heap *heap, const void *element, int priority);
bool heap_pull(heap *heap, void *out_element, int *out_priority);

size_t heap_size(const heap *heap);
