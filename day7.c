#include "arraylist.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int compare_int(const void *a, const void *b) {
    int aa = *(const int *)a;
    int bb = *(const int *)b;
    if (aa < bb)
        return -1;
    else if (aa > bb)
        return 1;
    else
        return 0;
}

static int fuel_cost2(int dist) { return dist * (dist + 1) / 2; }

int main() {
    arraylist positions;
    arraylist_init(&positions, sizeof(int));
    for (;;) {
        int position;
        int result = scanf("%d,", &position);
        if (result == EOF) {
            break;
        }
        assert(result == 1);
        arraylist_append(&positions, &position);
    }
    assert(arraylist_size(&positions) % 2 == 0);
    assert(arraylist_size(&positions) > 0);

    arraylist_sort(&positions, &compare_int);
    int middle = *(int *)arraylist_get(&positions, arraylist_size(&positions) / 2);
    int fuel = 0;
    for (int *position = arraylist_begin(&positions); position != arraylist_end(&positions); ++position) {
        fuel += abs(*position - middle);
    }
    printf("%d\n", fuel);

    int min_fuel = INT_MAX;
    int min_pos = *(int *)arraylist_get(&positions, 0);
    int max_pos = *(int *)arraylist_get(&positions, arraylist_size(&positions) - 1);
    for (int goal = min_pos; goal <= max_pos; ++goal) {
        int this_fuel = 0;
        for (int *position = arraylist_begin(&positions); position != arraylist_end(&positions); ++position) {
            this_fuel += fuel_cost2(abs(*position - goal));
        }
        if (this_fuel < min_fuel) {
            min_fuel = this_fuel;
        }
    }
    printf("%d\n", min_fuel);

    arraylist_free(&positions);
    return 0;
}