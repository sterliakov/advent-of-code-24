#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

// can't be const size_t: "VLA can't be initialized"
#define DIRECTIONS_COUNT 4

const char MOVE_MASK = (1 << DIRECTIONS_COUNT) - 1;
const char EMPTY = 1 << DIRECTIONS_COUNT;
const char WALL = 1 << (DIRECTIONS_COUNT + 1);

point_t board_translate(board_t board[static 1]) {
    size_t offset = (size_t)-1;
    for (char *ch = board->body + board_length(board) - 1; ch >= board->body;
         ch--) {
        switch (*ch) {
            case '^':
                offset = ch - board->body;
                // fallthrough
            case '.':
                *ch = EMPTY;
                break;
            case '#':
                *ch = WALL;
                break;
        }
    }
    assert(offset != (size_t)-1);
    return board_i2p(board, offset);
}

long traverse(board_t board[static 1], point_t pos) {
    static offset_t directions[DIRECTIONS_COUNT]
        = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    size_t dir_index = 0;
    offset_t dir = directions[dir_index];
    char dir_flag = (char)(1 << dir_index);

    long total = 0;
    char ch;
    while ((ch = board_at(board, pos)) != 0) {
        if (ch & WALL) {
            pos.c -= dir.dc;
            pos.r -= dir.dr;
            dir_index = (dir_index + 1) % DIRECTIONS_COUNT;
            dir = directions[dir_index];
            dir_flag = (char)(1 << dir_index);
            continue;
        }
        if ((ch & MOVE_MASK) == 0) {
            total++;
        } else if (ch & dir_flag) {
            // cycle found - been there in the same direction
            return -1;
        }
        board_set_unchecked(board, pos, (char)(dir_flag | ch));
        pos.c += dir.dc;
        pos.r += dir.dr;
    }
    return total;
}

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    point_t start = board_translate(&board);

    long total = traverse(&board, start);
    board_delete(&board);
    return total;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    point_t start = board_translate(&board);

    board_t board_copy;
    if (!board_clone(&board_copy, &board)) {
        perror("Failed to allocate memory for a board copy");
        board_delete(&board);
        return -1;
    }

    traverse(&board_copy, start);
    board_t orig_visits;
    if (!board_clone(&orig_visits, &board_copy)) {
        perror("Failed to allocate memory for a board copy");
        board_delete(&board);
        board_delete(&board_copy);
        return -1;
    }

    long total = 0;
    size_t board_size = board_length(&board);
    size_t start_index = board_p2i_unchecked(&board, start);
    for (size_t i = 0; i < board_size; i++) {
        if (board.body[i] != EMPTY || i == start_index
            || (orig_visits.body[i] & MOVE_MASK) == 0) {
            // Skip if there's already an obstacle or the cell is unreachable
            continue;
        }
        memcpy(board_copy.body, board.body, board_size * sizeof(char));
        board_copy.body[i] = WALL;
        if (traverse(&board_copy, start) == -1)
            total++;
    }

    board_delete(&board);
    board_delete(&board_copy);
    board_delete(&orig_visits);
    return total;
}
