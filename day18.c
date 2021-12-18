#include "arraylist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum node_type { PAIR, NUMBER } node_type;

typedef struct node {
    node_type type;
    union {
        struct {
            struct node *left;
            struct node *right;
        };
        int number;
    };
} node;

static void free_node(node *node) {
    if (node->type == PAIR) {
        free_node(node->left);
        free_node(node->right);
    }
    free(node);
}

static node *clone_node(node *node) {
    struct node *result = malloc(sizeof(*result));
    result->type = node->type;
    if (node->type == PAIR) {
        result->left = clone_node(node->left);
        result->right = clone_node(node->right);
    } else {
        result->number = node->number;
    }
    return result;
}

static node *parse_number() {
    char c = fgetc(stdin);
    if (c == '[') {
        node *node = malloc(sizeof(*node));
        node->type = PAIR;
        node->left = parse_number();
        c = fgetc(stdin);
        assert(c == ',');
        node->right = parse_number();
        c = fgetc(stdin);
        assert(c == ']');
        return node;
    } else if ('0' <= c && c <= '9') {
        int result = ungetc(c, stdin);
        assert(result == c);
        node *node = malloc(sizeof(*node));
        node->type = NUMBER;
        result = scanf("%d", &node->number);
        assert(result == 1);
        return node;
    } else {
        assert(false);
    }
}

static bool try_explode_rec(int depth, node *at, int **left_number, int *add_to_next_number) {
    if (at->type == PAIR) {
        if (depth == 4 && *add_to_next_number == -1) {
            assert(at->left->type == NUMBER);
            assert(at->right->type == NUMBER);
            if (*left_number) {
                **left_number += at->left->number;
            }
            *add_to_next_number = at->right->number;
            free_node(at->left);
            free_node(at->right);
            at->type = NUMBER;
            at->number = 0;
            return false;
        } else {
            return try_explode_rec(depth + 1, at->left, left_number, add_to_next_number) ||
                   try_explode_rec(depth + 1, at->right, left_number, add_to_next_number);
        }
    } else {
        if (*add_to_next_number != -1) {
            at->number += *add_to_next_number;
            return true;
        } else {
            *left_number = &at->number;
            return false;
        }
    }
}

static bool try_explode(node *root) {
    int *left_number = NULL;
    int add_to_next_number = -1;
    try_explode_rec(0, root, &left_number, &add_to_next_number);
    return add_to_next_number != -1;
}

static bool try_split(node *root) {
    if (root->type == PAIR) {
        return try_split(root->left) || try_split(root->right);
    } else if (root->number >= 10) {
        int number = root->number;
        root->type = PAIR;
        root->left = malloc(sizeof(*root->left));
        root->left->type = NUMBER;
        root->left->number = number / 2;
        root->right = malloc(sizeof(*root->right));
        root->right->type = NUMBER;
        root->right->number = number / 2 + number % 2;
        return true;
    } else {
        return false;
    }
}

static node *add(node *lhs, node *rhs) {
    node *result = malloc(sizeof(*result));
    result->type = PAIR;
    result->left = clone_node(lhs);
    result->right = clone_node(rhs);
    while (try_explode(result) || try_split(result))
        ;
    return result;
}

static int magnitude(node *node) {
    if (node->type == NUMBER) {
        return node->number;
    } else {
        return 3 * magnitude(node->left) + 2 * magnitude(node->right);
    }
}

int main() {
    arraylist numbers;
    arraylist_init(&numbers, sizeof(node *));
    while (!feof(stdin)) {
        node *number = parse_number();
        arraylist_append(&numbers, &number);
        int result = scanf("\n");
        assert(result == 0);
    }

    node *result = clone_node(*(node **)arraylist_get(&numbers, 0));
    for (node **node = (struct node **)arraylist_begin(&numbers) + 1; node != arraylist_end(&numbers); ++node) {
        struct node *next_result = add(result, *node);
        free_node(result);
        result = next_result;
    }
    printf("%d\n", magnitude(result));

    int max_magnitude = 0;
    for (node **a = arraylist_begin(&numbers); a != arraylist_end(&numbers); ++a) {
        for (node **b = arraylist_begin(&numbers); b != arraylist_end(&numbers); ++b) {
            node *result = add(*a, *b);
            int this_magnitude = magnitude(result);
            free_node(result);
            if (this_magnitude > max_magnitude) {
                max_magnitude = this_magnitude;
            }
        }
    }
    printf("%d\n", max_magnitude);

    free_node(result);
    for (node **node = arraylist_begin(&numbers); node != arraylist_end(&numbers); ++node) {
        free_node(*node);
    }
    arraylist_free(&numbers);
    return 0;
}