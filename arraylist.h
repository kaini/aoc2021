/**
 * Simple array list.
 * Items must be copyable with memcpy and will not be free'd by the container.
 */

#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef struct arraylist {
    size_t size;
    size_t capacity;
    size_t item_size;
    char *buffer;
} arraylist;

void arraylist_init(arraylist *arraylist, size_t item_size);
void arraylist_free(arraylist *arraylist);

void arraylist_append(arraylist *arraylist, const void *item);
void arraylist_filter_destroy_order(arraylist *arraylist, bool (*filter)(void *item, void *ctx), void *ctx);
void arraylist_sort(arraylist *arraylist, int (*cmp)(const void *a, const void *b));

void *arraylist_get(const arraylist *arraylist, size_t index);
size_t arraylist_size(const arraylist *arraylist);
void *arraylist_begin(const arraylist *arraylist);
void *arraylist_end(const arraylist *arraylist);
