#include "arraylist.h"
#include "parse.h"
#include <stdio.h>
#include <assert.h>

#define NUMBER_STR_LEN 20
#define NUMBER_STR_FORMAT "%19s"
typedef char number_str[NUMBER_STR_LEN];

static int count_ones(arraylist* numbers, size_t index) {
    int result = 0;
    for (number_str* number = arraylist_begin(numbers); number != arraylist_end(numbers); ++number) {
        if ((*number)[index] == '1') {
            result += 1;
        }
    }
    return result;
}

typedef struct filter_ch_ctx {
    char ch;
    size_t index;
} filter_ch_ctx;

static bool filter_ch(void* item_ptr, void* ctx_ptr) {
    number_str* item = item_ptr;
    filter_ch_ctx* ctx = ctx_ptr;
    return (*item)[ctx->index] == ctx->ch;
}

static int find_rating(arraylist* orig_numbers, bool oxygen) {
    arraylist numbers;
    arraylist_init(&numbers, sizeof(number_str));
    for (number_str* item = arraylist_begin(orig_numbers); item != arraylist_end(orig_numbers); ++item) {
        arraylist_append(&numbers, item);
    }

    size_t i = 0;
    while (arraylist_size(&numbers) > 1) {
        int ones = count_ones(&numbers, i);
        int zeros = arraylist_size(&numbers) - ones;
        char ch;
        if (ones >= zeros) {
            ch = oxygen ? '1' : '0';
        } else if (ones < zeros) {
            ch = oxygen ? '0' : '1';
        }
        arraylist_filter_destroy_order(&numbers, filter_ch, &(filter_ch_ctx){ .ch = ch, .index = i });
        i += 1;
        assert(i < NUMBER_STR_LEN);
    }
    assert(arraylist_size(&numbers) == 1);

    int result = parse_binary(arraylist_get(&numbers, 0));
    arraylist_free(&numbers);
    return result;
}

int main() {
    arraylist numbers;
    arraylist_init(&numbers, sizeof(number_str));
    for (;;) {
        number_str number;
        int result = scanf(NUMBER_STR_FORMAT, number);
        if (result == EOF) {
            break;
        }
        assert(result == 1);
        arraylist_append(&numbers, &number);
    }

    int ones_count[NUMBER_STR_LEN] = { 0 };
    int zero_count[NUMBER_STR_LEN] = { 0 };
    for (number_str* number = arraylist_begin(&numbers); number != arraylist_end(&numbers); ++number) {
        size_t i = 0;
        for (char* nc = *number; *nc; ++nc, ++i) {
            if (*nc == '1') {
                ones_count[i] += 1;
            } else {
                zero_count[i] += 1;
            }
        }
    }
    number_str gamma;
    number_str delta;
    for (size_t i = 0; i < NUMBER_STR_LEN; ++i) {
        if (ones_count[i] > zero_count[i]) {
            gamma[i] = '1';
            delta[i] = '0';
        } else if (ones_count[i] < zero_count[i]) {
            gamma[i] = '0';
            delta[i] = '1';
        } else {
            gamma[i] = '\0';
            delta[i] = '\0';
        }
    }
    int gamma_int = parse_binary(gamma);
    int delta_int = parse_binary(delta);
    printf("%d\n", gamma_int * delta_int);

    int oxygen = find_rating(&numbers, true);
    int co2 = find_rating(&numbers, false);
    printf("%d\n", oxygen * co2);

    arraylist_free(&numbers);
    return 0;
}
