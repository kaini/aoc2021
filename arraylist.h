#pragma once
#include <stddef.h>

typedef struct arraylist {
    size_t size;
    size_t capacity;
    size_t item_size;
    char* buffer;
} arraylist;

typedef void* arraylist_iter;

void arraylist_init(arraylist* arraylist, size_t item_size);
void arraylist_free(arraylist* arraylist);

void arraylist_append(arraylist* arraylist, void* item);

void* arraylist_get(arraylist* arraylist, size_t index);
size_t arraylist_size(arraylist* arraylist);
void* arraylist_array(arraylist* arraylist);
