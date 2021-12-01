#include "arraylist.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void arraylist_init(arraylist* arraylist, size_t item_size) {
    assert(arraylist);
    assert(item_size > 0);
    arraylist->size = 0;
    arraylist->capacity = 0;
    arraylist->item_size = item_size;
    arraylist->buffer = NULL;
}

void arraylist_free(arraylist* arraylist) {
    assert(arraylist);
    free(arraylist->buffer);
}

void arraylist_append(arraylist* arraylist, void* item) {
    assert(arraylist);
    assert(item);
    if (arraylist->size == arraylist->capacity) {
        arraylist->capacity = (arraylist->capacity + 1) * 2;
        arraylist->buffer = realloc(arraylist->buffer, arraylist->capacity * arraylist->item_size);
        assert(arraylist->buffer);
    }
    memcpy(arraylist->buffer + arraylist->size * arraylist->item_size, item, arraylist->item_size);
    arraylist->size += 1;
}

void* arraylist_get(arraylist* arraylist, size_t index) {
    assert(arraylist);
    assert(index < arraylist->size);
    return arraylist->buffer + arraylist->item_size * index;
}

size_t arraylist_size(arraylist* arraylist) {
    assert(arraylist);
    return arraylist->size;
}

void* arraylist_array(arraylist* arraylist) {
    assert(arraylist);
    return arraylist->buffer;
}
