#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"
#include "../lib/utils.c"

typedef struct __attribute__((aligned(32))) cell_t {
    char val;
    point_t pos;
} cell_t;

int cell_cmp(const void *lhs, const void *rhs) {
    const cell_t *const a = lhs;
    const cell_t *const b = rhs;
    return (a->val > b->val) - (a->val < b->val);
}

size_t unpack_board(const board_t board[static 1], cell_t cells[static 1]) {
    size_t count = 0;
    for (size_t r = 0; r < board->height; r++) {
        for (size_t c = 0; c < board->width; c++) {
            char val = board_at(board, (point_t){r, c});
            if (val != '.') {
                cells[count++] = (cell_t){val, (point_t){r, c}};
            }
        }
    }
    return count;
}

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    size_t len = board_length(&board);

    cell_t *cells = malloc(sizeof(cell_t) * len);
    if (cells == NULL) {
        perror("malloc failed");
        board_delete(&board);
        return -1;
    }
    size_t count = unpack_board(&board, cells);
    qsort(cells, count, sizeof(cell_t), cell_cmp);

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count && cells[i].val == cells[j].val; j++) {
            ptrdiff_t dr = (ptrdiff_t)(cells[i].pos.r - cells[j].pos.r);
            ptrdiff_t dc = (ptrdiff_t)(cells[i].pos.c - cells[j].pos.c);
            board_set(
                &board, (point_t){cells[i].pos.r + dr, cells[i].pos.c + dc}, '#'
            );
            board_set(
                &board, (point_t){cells[j].pos.r - dr, cells[j].pos.c - dc}, '#'
            );
        }
    }

    long total = 0;
    for (size_t i = 0; i < len; i++) {
        total += board.body[i] == '#';
    }

    board_delete(&board);
    free(cells);
    return total;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    size_t len = board_length(&board);

    cell_t *cells = malloc(sizeof(cell_t) * len);
    if (cells == NULL) {
        perror("malloc failed");
        board_delete(&board);
        return -1;
    }
    size_t count = unpack_board(&board, cells);
    qsort(cells, count, sizeof(cell_t), cell_cmp);

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count && cells[i].val == cells[j].val; j++) {
            ptrdiff_t dr = (ptrdiff_t)(cells[i].pos.r - cells[j].pos.r);
            ptrdiff_t dc = (ptrdiff_t)(cells[i].pos.c - cells[j].pos.c);
            ptrdiff_t g = gcd(dr, dc);
            dr /= g;
            dc /= g;
            point_t start = cells[i].pos;
            while (board_at(&board, start) > 0) {
                board_set(&board, start, '#');
                start.r += dr;
                start.c += dc;
            }
            start = cells[i].pos;
            while (board_at(&board, start) > 0) {
                board_set(&board, start, '#');
                start.r -= dr;
                start.c -= dc;
            }
        }
    }

    long total = 0;
    for (size_t i = 0; i < len; i++) {
        total += board.body[i] == '#';
    }

    board_delete(&board);
    free(cells);
    return total;
}
