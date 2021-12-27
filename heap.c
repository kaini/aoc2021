#include "heap.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void heap_init(heap *heap, size_t element_size, size_t (*hash_element)(const void *),
               bool (*equal_element)(const void *, const void *)) {
    heap->element_size = element_size;
    heap->capacity = 0;
    map_init(&heap->indices, element_size, hash_element, equal_element, sizeof(size_t));
    heap->buffer = NULL;
    heap->prios = NULL;
}

void heap_free(heap *heap) {
    map_free(&heap->indices);
    free(heap->buffer);
    free(heap->prios);
}

void heap_insert_or_decrease(heap *heap, const void *element, int priority) {
    if (map_size(&heap->indices) == heap->capacity) {
        heap->capacity = (heap->capacity + 1) * 2;
        heap->buffer = realloc(heap->buffer, heap->element_size * heap->capacity);
        assert(heap->buffer);
        heap->prios = realloc(heap->prios, sizeof(int) * heap->capacity);
        assert(heap->prios);
    }

    size_t at;
    size_t *index = map_get(&heap->indices, element);
    if (!index) {
        at = map_size(&heap->indices);
        map_put(&heap->indices, element, &at);
    } else if (heap->prios[*index] <= priority) {
        // do not insert if this would be an increase
        return;
    } else {
        at = *index;
    }
    memcpy(heap->buffer + at * heap->element_size, element, heap->element_size);
    heap->prios[at] = priority;

    while (at > 0) {
        size_t parent = (at - 1) / 2;
        if (heap->prios[parent] > heap->prios[at]) {
            int tmp = heap->prios[parent];
            heap->prios[parent] = heap->prios[at];
            heap->prios[at] = tmp;

            char buffer[heap->element_size];
            memcpy(buffer, heap->buffer + parent * heap->element_size, heap->element_size);
            memcpy(heap->buffer + parent * heap->element_size, heap->buffer + at * heap->element_size,
                   heap->element_size);
            memcpy(heap->buffer + at * heap->element_size, buffer, heap->element_size);

            map_put(&heap->indices, heap->buffer + parent * heap->element_size, &parent);
            map_put(&heap->indices, heap->buffer + at * heap->element_size, &at);

            at = parent;
        } else {
            break;
        }
    }
}

bool heap_pull(heap *heap, void *out_element, int *out_priority) {
    const size_t zero = 0;

    if (map_size(&heap->indices) == 0) {
        return false;
    }

    memcpy(out_element, heap->buffer, heap->element_size);
    *out_priority = heap->prios[0];
    map_iter result = map_remove(&heap->indices, heap->buffer);
    assert(result != map_end(&heap->indices));
    if (map_size(&heap->indices) == 0) {
        return true;
    }

    memcpy(heap->buffer, heap->buffer + map_size(&heap->indices) * heap->element_size, heap->element_size);
    heap->prios[0] = heap->prios[map_size(&heap->indices)];
    map_put(&heap->indices, heap->buffer, &zero);

    size_t at = 0;
    for (;;) {
        size_t a = 2 * at + 1;
        if (a >= map_size(&heap->indices)) {
            break;
        }
        size_t b = 2 * at + 2;
        size_t dest = (b >= map_size(&heap->indices) || heap->prios[a] < heap->prios[b]) ? a : b;

        if (heap->prios[dest] < heap->prios[at]) {
            int tmp = heap->prios[at];
            heap->prios[at] = heap->prios[dest];
            heap->prios[dest] = tmp;

            char buffer[heap->element_size];
            memcpy(buffer, heap->buffer + at * heap->element_size, heap->element_size);
            memcpy(heap->buffer + at * heap->element_size, heap->buffer + dest * heap->element_size,
                   heap->element_size);
            memcpy(heap->buffer + dest * heap->element_size, buffer, heap->element_size);

            map_put(&heap->indices, heap->buffer + at * heap->element_size, &at);
            map_put(&heap->indices, heap->buffer + dest * heap->element_size, &dest);

            at = dest;
        } else {
            break;
        }
    }

    return true;
}

size_t heap_size(const heap *heap) { return map_size(&heap->indices); }
