#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    long total = 0;
    const size_t arrsize = 4;
    char slice[arrsize];
    for (size_t i = 0; i < board.height; i++) {
        for (size_t j = 0; j < board.width; j++) {
            if (board_at(&board, (point_t){i, j}) != 'X') {
                continue;
            }
            for (char *c = "AWDXQEZC"; *c; c++) {
                if (board_slice(&board, (point_t){i, j}, *c, arrsize, slice)) {
                    total += (strncmp(slice, "XMAS", arrsize) == 0);
                }
            }
        }
    }

    board_delete(&board);
    return total;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    long total = 0;
    const size_t arrsize = 3;
    char slice[arrsize];
    // Search for a midpoint (always 'A') and compare both diagonals
    for (size_t i = 1; i < board.height - 1; i++) {
        for (size_t j = 1; j < board.width - 1; j++) {
            if (board_at(&board, (point_t){i, j}) != 'A') {
                continue;
            }
            if (board_slice(
                    &board, (point_t){i - 1, j - 1}, 'C', arrsize, slice
                )
                && (strncmp(slice, "MAS", arrsize) == 0
                    || strncmp(slice, "SAM", arrsize) == 0)
                && board_slice(
                    &board, (point_t){i - 1, j + 1}, 'Z', arrsize, slice
                )
                && (strncmp(slice, "MAS", arrsize) == 0
                    || strncmp(slice, "SAM", arrsize) == 0)) {
                total++;
            }
        }
    }

    board_delete(&board);
    return total;
}
