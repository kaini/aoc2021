#include "arraylist.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 5
typedef int board[BOARD_SIZE][BOARD_SIZE];

static void make_mark(board* mark, board* board, int number) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((*board)[i][j] == number) {
                (*mark)[i][j] = 1;
            }
        }
    }
}

static bool check_won(board* mark) {
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

static int board_sum(board* mark, board* board, int number) {
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
    arraylist_init(&numbers, sizeof(int), NULL);
    for (;;) {
        int c = fgetc(stdin);
        if (c == EOF || c == '\n') break;
        int result = ungetc(c, stdin);
        assert(result != EOF);

        int number;
        result = scanf("%d,", &number);
        assert(result == 1);
        arraylist_append(&numbers, &number);
    }

    arraylist boards;
    arraylist_init(&boards, sizeof(board), NULL);
    for (;;) {
        board board;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                int result = scanf("%d", &board[i][j]);
                if (result == EOF) {
                    assert(i == 0 && j == 0);
                    goto after_read_loop;
                }
                assert(result == 1);
            }
        }
        arraylist_append(&boards, &board);
    }
    after_read_loop:;

    arraylist marks;
    arraylist_init(&marks, sizeof(board), NULL);
    for (size_t i = 0; i < arraylist_size(&boards); ++i) {
        board board = { 0 };
        arraylist_append(&marks, &board);
    }

    size_t winning_board_index = -1;
    board* winning_board = NULL;
    board* winning_mark = NULL;
    int* winning_number = NULL;
    for (int* number = arraylist_begin(&numbers); number != arraylist_end(&numbers); ++number) {
        for (size_t b = 0; b < arraylist_size(&boards); ++b) {
            board* mark = arraylist_get(&marks, b);
            board* board = arraylist_get(&boards, b);
            make_mark(mark, board, *number);
            if (check_won(mark)) {
                winning_board_index = b;
                winning_board = board;
                winning_mark = mark;
                winning_number = number;
                goto after_game_loop;
            }
        }
    }
    after_game_loop:;
    assert(winning_board && winning_mark && winning_number && winning_board_index != (size_t)-1);
    printf("%d\n", board_sum(winning_mark, winning_board, *winning_number));

    size_t boards_won_count = 1;
    bool boards_won[arraylist_size(&boards)];
    memset(&boards_won, 0, sizeof(bool) * arraylist_size(&boards));
    boards_won[winning_board_index] = true;
    for (int* number = winning_number; number != arraylist_end(&numbers) && boards_won_count < arraylist_size(&boards); ++number) {
        for (size_t b = 0; b < arraylist_size(&boards); ++b) {
            if (!boards_won[b]) {
                board* mark = arraylist_get(&marks, b);
                board* board = arraylist_get(&boards, b);
                make_mark(mark, board, *number);
                if (check_won(mark)) {
                    winning_board_index = b;
                    winning_board = board;
                    winning_mark = mark;
                    winning_number = number;
                    boards_won_count += 1;
                    boards_won[b] = true;
                }
            }
        }
    }
    assert(boards_won_count == arraylist_size(&boards));
    printf("%d\n", board_sum(winning_mark, winning_board, *winning_number));

    arraylist_free(&marks);
    arraylist_free(&boards);
    arraylist_free(&numbers);
    return 0;
}
