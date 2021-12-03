#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct arraylist {
    size_t size;
    size_t capacity;
    size_t item_size;
    void (*free_item)(void* item);
    char* buffer;
} arraylist;

void arraylist_init(arraylist* arraylist, size_t item_size, void (*free_item)(void* item));
void arraylist_free(arraylist* arraylist);

void arraylist_append(arraylist* arraylist, const void* item);
void arraylist_filter_destroy_order(arraylist* arraylist, bool (*filter)(void* item, void* ctx), void* ctx);

void* arraylist_get(const arraylist* arraylist, size_t index);
size_t arraylist_size(const arraylist* arraylist);
void* arraylist_begin(const arraylist* arraylist);
void* arraylist_end(const arraylist* arraylist);
