#include "arraylist.h"
#include <assert.h>
#include <stdio.h>

static int cmp_ll(const void *a, const void *b) {
    long long aa = *(const long long *)a;
    long long bb = *(const long long *)b;
    if (aa > bb) {
        return 1;
    } else if (aa < bb) {
        return -1;
    } else {
        return 0;
    }
}

int main() {
    int score = 0;
    arraylist stack;
    arraylist_init(&stack, sizeof(int));
    arraylist completion_scores;
    arraylist_init(&completion_scores, sizeof(long long));

    for (;;) {
        int c = getc(stdin);

        if (c == EOF) {
            break;
        }

        if (c == '\r' || c == '\n') {
            long long completion_score = 0;
            int popc;
            while (arraylist_pop(&stack, &popc)) {
                completion_score *= 5;
                switch (popc) {
                case '<':
                    completion_score += 4;
                    break;
                case '{':
                    completion_score += 3;
                    break;
                case '[':
                    completion_score += 2;
                    break;
                case '(':
                    completion_score += 1;
                    break;
                default:
                    assert(false);
                    break;
                }
            }
            if (completion_score > 0) {
                arraylist_append(&completion_scores, &completion_score);
            }

            continue;
        }

        int popc;
        switch (c) {
        case '<':
        case '(':
        case '[':
        case '{':
            arraylist_append(&stack, &c);
            break;
        case '>':
            if (!arraylist_pop(&stack, &popc)) {
                assert(false);
            }
            if (popc != '<') {
                score += 25137;
                (void)!scanf("%*s");
                arraylist_clear(&stack);
            }
            break;
        case ']':
            if (!arraylist_pop(&stack, &popc)) {
                assert(false);
            }
            if (popc != '[') {
                score += 57;
                (void)!scanf("%*s");
                arraylist_clear(&stack);
            }
            break;
        case ')':
            if (!arraylist_pop(&stack, &popc)) {
                assert(false);
            }
            if (popc != '(') {
                score += 3;
                (void)!scanf("%*s");
                arraylist_clear(&stack);
            }
            break;
        case '}':
            if (!arraylist_pop(&stack, &popc)) {
                assert(false);
            }
            if (popc != '{') {
                score += 1197;
                (void)!scanf("%*s");
                arraylist_clear(&stack);
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    printf("%d\n", score);

    arraylist_sort(&completion_scores, cmp_ll);
    assert(arraylist_size(&completion_scores) % 2 == 1);
    printf("%lld\n", *(long long *)arraylist_get(&completion_scores, arraylist_size(&completion_scores) / 2));

    arraylist_free(&completion_scores);
    arraylist_free(&stack);
    return 0;
}