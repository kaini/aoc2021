#include "arraylist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 5
typedef int board[BOARD_SIZE][BOARD_SIZE];

static void read_boards(arraylist *boards) {
    for (;;) {
        board board;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                int result = scanf("%d", &board[i][j]);
                if (result == EOF) {
                    assert(i == 0 && j == 0);
                    return;
                }
                assert(result == 1);
            }
        }
        arraylist_append(boards, &board);
    }
}

static void make_mark(board *mark, board *board, int number) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((*board)[i][j] == number) {
                (*mark)[i][j] = 1;
            }
        }
    }
}

static bool check_won(board *mark) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        bool row = true;
        bool col = true;
        for (int j = 0; j < BOARD_SIZE && (row || col); ++j) {
            row = row && (*mark)[i][j];
            col = col && (*mark)[j][i];
        }
        if (row || col) {
            return true;
        }
    }
    return false;
}

static int board_sum(board *mark, board *board, int number) {
    int result = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (!(*mark)[i][j]) {
                result += (*board)[i][j];
            }
        }
    }
    result *= number;
    return result;
}

int main() {
    arraylist numbers;
    arraylist_init(&numbers, sizeof(int));
    for (;;) {
        int c = fgetc(stdin);
        if (c == EOF || c == '\n')
            break;
        int result = ungetc(c, stdin);
        assert(result != EOF);

        int number;
        result = scanf("%d,", &number);
        assert(result == 1);
        arraylist_append(&numbers, &number);
    }

    arraylist boards;
    arraylist_init(&boards, sizeof(board));
    read_boards(&boards);

    arraylist marks;
    arraylist_init(&marks, sizeof(board));
    for (size_t i = 0; i < arraylist_size(&boards); ++i) {
        board board = {0};
        arraylist_append(&marks, &board);
    }

    bool *boards_won = calloc(arraylist_size(&boards), sizeof(bool));
    assert(boards_won);
    size_t won_count = 0;
    for (int *number = arraylist_begin(&numbers); number != arraylist_end(&numbers); ++number) {
        for (size_t b = 0; b < arraylist_size(&boards); ++b) {
            if (!boards_won[b]) {
                board *mark = arraylist_get(&marks, b);
                board *board = arraylist_get(&boards, b);
                make_mark(mark, board, *number);
                if (check_won(mark)) {
                    if (won_count == 0 || won_count == arraylist_size(&boards) - 1) {
                        printf("%d\n", board_sum(mark, board, *number));
                    }
                    won_count += 1;
                    boards_won[b] = true;
                }
            }
        }
    }

    free(boards_won);
    arraylist_free(&marks);
    arraylist_free(&boards);
    arraylist_free(&numbers);
    return 0;
}
