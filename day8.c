#include "arraylist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef bool segments[7];

static const segments numbers[10] = {
    /*
     a  b  c  d  e  f  g
    */
    {1, 1, 1, 0, 1, 1, 1}, // 0
    {0, 0, 1, 0, 0, 1, 0}, // 1
    {1, 0, 1, 1, 1, 0, 1}, // 2
    {1, 0, 1, 1, 0, 1, 1}, // 3
    {0, 1, 1, 1, 0, 1, 0}, // 4
    {1, 1, 0, 1, 0, 1, 1}, // 5
    {1, 1, 0, 1, 1, 1, 1}, // 6
    {1, 0, 1, 0, 0, 1, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};

typedef int mapping[7];

static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static void generate_all_mappings_rec(arraylist *mappings, int at, mapping *current) {
    if (at == 6) {
        arraylist_append(mappings, current);
    } else {
        for (int i = at; i < 7; ++i) {
            swap(&(*current)[at], &(*current)[i]);
            generate_all_mappings_rec(mappings, at + 1, current);
            swap(&(*current)[at], &(*current)[i]);
        }
    }
}

static void generate_all_mappings(arraylist *mappings) {
    arraylist_init(mappings, sizeof(mapping));
    mapping current = {0, 1, 2, 3, 4, 5, 6};
    generate_all_mappings_rec(mappings, 0, &current);
}

static bool is_number(segments *segments, mapping *mapping, int number) {
    assert(0 <= number && number <= 9);
    for (int s = 0; s < 7; ++s) {
        if ((*segments)[(*mapping)[s]] != numbers[number][s]) {
            return false;
        }
    }
    return true;
}

static int find_number(segments *segments, mapping *mapping) {
    for (int n = 0; n <= 9; ++n) {
        if (is_number(segments, mapping, n)) {
            return n;
        }
    }
    return -1;
}

static bool mapping_matches(arraylist *line, mapping *mapping) {
    for (segments *segments = arraylist_begin(line); segments != arraylist_end(line); ++segments) {
        if (find_number(segments, mapping) == -1) {
            return false;
        }
    }
    return true;
}

int main() {
    arraylist lines;
    arraylist_init(&lines, sizeof(arraylist));
    arraylist line;
    arraylist_init(&line, sizeof(segments));
    for (;;) {
        int c = getc(stdin);
        if (c == EOF || c == '\n') {
            if (arraylist_size(&line) == 0) {
                arraylist_free(&line);
            } else {
                arraylist_append(&lines, &line);
            }
            if (c == EOF) {
                break;
            }
            arraylist_init(&line, sizeof(segments));
        } else {
            int result = ungetc(c, stdin);
            assert(result == c);
        }

        char str[8];
        int result = scanf("%7s", str);
        if (result == EOF) {
            continue;
        }
        assert(result == 1);
        if (str[0] != '|') {
            segments segments = {0};
            for (char *c = str; *c; ++c) {
                assert('a' <= *c && *c <= 'g');
                segments[*c - 'a'] = true;
            }
            arraylist_append(&line, &segments);
        }
    }

    arraylist all_mappings;
    generate_all_mappings(&all_mappings);

    int count = 0;
    int sum = 0;
    for (arraylist *line = arraylist_begin(&lines); line != arraylist_end(&lines); ++line) {
        for (mapping *mapping = arraylist_begin(&all_mappings); mapping != arraylist_end(&all_mappings); ++mapping) {
            if (mapping_matches(line, mapping)) {
                size_t size = arraylist_size(line);
                assert(size >= 4);
                for (int i = 0, p = 1; i < 4; i += 1, p *= 10) {
                    int digit = find_number(arraylist_get(line, size - i - 1), mapping);
                    count += digit == 1 || digit == 4 || digit == 7 || digit == 8;
                    sum += digit * p;
                }
                break;
            }
        }
    }
    printf("%d\n%d\n", count, sum);

    arraylist_free(&all_mappings);
    for (arraylist *line = arraylist_begin(&lines); line != arraylist_end(&lines); ++line) {
        arraylist_free(line);
    }
    arraylist_free(&lines);
    return 0;
}