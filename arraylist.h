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

void arraylist_append(arraylist* arraylist, const void* item);

void* arraylist_get(const arraylist* arraylist, size_t index);
size_t arraylist_size(const arraylist* arraylist);
void* arraylist_begin(const arraylist* arraylist);
void* arraylist_end(const arraylist* arraylist);
