#include "arraylist.h"
#include "map.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

static long zero = 0;

typedef char key[2];

static size_t hash_key(const void *key_ptr) {
    const key *key_p = (const key *)key_ptr;
    return (*key_p)[0] * 2342 + (*key_p)[1] * 3453;
}

static bool eq_key(const void *a_ptr, const void *b_ptr) {
    const key *a = (const key *)a_ptr;
    const key *b = (const key *)b_ptr;
    return (*a)[0] == (*b)[0] && (*a)[1] == (*b)[1];
}

static void map_add(map *map, key *key, long amount) {
    long *value = map_get(map, key);
    if (!value) {
        value = map_put(map, key, &zero);
    }
    *value += amount;
}

static void apply_rules(map *source, map *dest, map *rules) {
    for (map_iter iter = map_begin(source); iter != map_end(source); iter = map_advance(source, iter)) {
        key *pair = map_key(source, iter);
        long count = *(long *)map_value(source, iter);
        char *rule = map_get(rules, pair);
        if (rule) {
            key pair1 = {(*pair)[0], *rule};
            map_add(dest, &pair1, count);
            key pair2 = {*rule, (*pair)[1]};
            map_add(dest, &pair2, count);
        } else {
            map_add(dest, pair, count);
        }
    }
}

static void find_result(arraylist *template, map *result) {
    long counts[26] = {0};
    for (map_iter iter = map_begin(result); iter != map_end(result); iter = map_advance(result, iter)) {
        key *pair = (key *)map_key(result, iter);
        long amount = *(long *)map_value(result, iter);
        counts[(*pair)[0] - 'A'] += amount;
        counts[(*pair)[1] - 'A'] += amount;
    }
    counts[*(char *)arraylist_get(template, 0) - 'A'] += 1;
    counts[*(char *)arraylist_get(template, arraylist_size(template) - 1) - 'A'] += 1;

    int min_char = 0;
    int max_char = 0;
    for (int c = 1; c < 26; ++c) {
        if (counts[c] == 0) {
            continue;
        }
        if (counts[c] < counts[min_char] || counts[min_char] == 0) {
            min_char = c;
        }
        if (counts[c] > counts[max_char] || counts[max_char] == 0) {
            max_char = c;
        }
    }

    printf("%ld\n", (counts[max_char] - counts[min_char]) / 2);
}

int main() {
    arraylist template;
    arraylist_init(&template, sizeof(char));
    for (;;) {
        int c = getc(stdin);
        if (c == EOF || c == '\n') {
            getc(stdin); // eat extra newline
            break;
        }
        char cc = c;
        arraylist_append(&template, &cc);
    }

    map rules;
    map_init(&rules, sizeof(key), hash_key, eq_key, sizeof(char));
    for (;;) {
        key key;
        char dest;
        int result = scanf("%c%c -> %c\n", &key[0], &key[1], &dest);
        if (result == EOF) {
            break;
        }
        assert(result == 3);
        map_put(&rules, &key, &dest);
    }

    map pairs1;
    map_init(&pairs1, sizeof(key), hash_key, eq_key, sizeof(long));
    map pairs2;
    map_init(&pairs2, sizeof(key), hash_key, eq_key, sizeof(long));
    map *source = &pairs1;
    map *dest = &pairs2;

    // initialize the pairs -> all chars are counted double except the first and last!
    char prev_c = *(char *)arraylist_get(&template, 0);
    for (char *c = (char *)arraylist_begin(&template) + 1; c != arraylist_end(&template); ++c) {
        key pair = {prev_c, *c};
        map_add(source, &pair, 1);
        prev_c = *c;
    }

    for (int i = 0; i < 40; ++i) {
        map_clear(dest);
        apply_rules(source, dest, &rules);

        map *temp = source;
        source = dest;
        dest = temp;

        if (i == 9 || i == 39) {
            find_result(&template, source);
        }
    }

    map_free(&pairs2);
    map_free(&pairs1);
    map_free(&rules);
    arraylist_free(&template);
    return 0;
}
