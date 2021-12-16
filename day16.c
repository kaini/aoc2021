#include "arraylist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct header {
    int version;
    int type;
} header;

typedef struct group {
    bool has_next;
    int number;
} group;

typedef struct output {
    int version_sum;
    long long operator_result;
} output;

static bool parse_bit(arraylist *input, size_t *at) { return *(bool *)arraylist_get(input, (*at)++); }

static int parse_int(arraylist *input, size_t *at, int size) {
    int result = 0;
    for (int i = 0; i < size; ++i) {
        result <<= 1;
        result |= parse_bit(input, at);
    }
    return result;
}

static header parse_header(arraylist *input, size_t *at) {
    header result;
    result.version = parse_int(input, at, 3);
    result.type = parse_int(input, at, 3);
    return result;
}

static group parse_group(arraylist *input, size_t *at) {
    group result;
    result.has_next = parse_bit(input, at);
    result.number = parse_int(input, at, 4);
    return result;
}

static long long parse_groups(arraylist *input, size_t *at) {
    group group = {.has_next = true, .number = 0};
    long long result = 0;
    while (group.has_next) {
        group = parse_group(input, at);
        result <<= 4;
        result |= group.number;
    }
    return result;
}

static void parse_padding(arraylist *input, size_t *at) {
    while (*at < arraylist_size(input)) {
        bool bit = parse_bit(input, at);
        assert(bit == 0);
    }
}

static long long apply_operator(int operator, long long current, long long next) {
    if (current == -1) {
        return next;
    } else {
        switch (operator) {
        case 0:
            return current + next;
        case 1:
            return current * next;
        case 2:
            return current < next ? current : next;
        case 3:
            return current > next ? current : next;
        case 5:
            return current > next;
        case 6:
            return current < next;
        case 7:
            return current == next;
        default:
            assert(false);
        }
    }
}

static output parse_package(arraylist *input, size_t *at) {
    header header = parse_header(input, at);
    output result = {.version_sum = header.version, .operator_result = -1};
    switch (header.type) {
    case 4:
        result.operator_result = parse_groups(input, at);
        break;
    default: {
        bool length_in_packets = parse_bit(input, at);
        if (length_in_packets) {
            int packets = parse_int(input, at, 11);
            for (int i = 0; i < packets; ++i) {
                output sub_result = parse_package(input, at);
                result.version_sum += sub_result.version_sum;
                result.operator_result =
                    apply_operator(header.type, result.operator_result, sub_result.operator_result);
            }
        } else {
            int bits = parse_int(input, at, 15);
            size_t end_bit = *at + bits;
            while (*at != end_bit) {
                assert(*at < end_bit);
                output sub_result = parse_package(input, at);
                result.version_sum += sub_result.version_sum;
                result.operator_result =
                    apply_operator(header.type, result.operator_result, sub_result.operator_result);
            }
        }
        break;
    }
    }
    return result;
}

int main() {
    arraylist input;
    arraylist_init(&input, sizeof(bool));
    for (;;) {
        unsigned char c;
        int result = scanf("%1hhx", &c);
        if (result == EOF) {
            break;
        }
        assert(result == 1);

        bool bit;
        bit = c & (1 << 3);
        arraylist_append(&input, &bit);
        bit = c & (1 << 2);
        arraylist_append(&input, &bit);
        bit = c & (1 << 1);
        arraylist_append(&input, &bit);
        bit = c & (1 << 0);
        arraylist_append(&input, &bit);
    }

    size_t at = 0;
    output result = parse_package(&input, &at);
    printf("%d\n%lld\n", result.version_sum, result.operator_result);
    parse_padding(&input, &at);

    arraylist_free(&input);
    return 0;
}
