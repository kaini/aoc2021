#include "arraylist.h"
#include <stdio.h>
#include <limits.h>
#include <assert.h>

int main() {
    arraylist depths;
    arraylist_init(&depths, sizeof(int), NULL);
    for (;;) {
        int depth;
        int result = scanf("%d", &depth);
        if (result == EOF) {
            break;
        }
        assert(result == 1);
        arraylist_append(&depths, &depth);
    }

    int count = 0;
    int prev_depth = INT_MAX;
    for (size_t i = 0; i < arraylist_size(&depths); ++i) {
        int depth = *(int*)arraylist_get(&depths, i);
        if (depth > prev_depth) {
            count += 1;
        }
        prev_depth = depth;
    }
    printf("%d\n", count);

    count = 0;
    int prev_sum = INT_MAX;
    int sum = 0;
    for (size_t i = 0; i < arraylist_size(&depths); ++i) {
        sum += *(int*)arraylist_get(&depths, i);
        if (i >= 3) sum -= *(int*)arraylist_get(&depths, i - 3);
        if (i >= 2) {
            if (sum > prev_sum) count += 1;
            prev_sum = sum; 
        }
    }
    printf("%d\n", count);

    arraylist_free(&depths);
    return 0;
}
