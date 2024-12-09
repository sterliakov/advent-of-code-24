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

bool board_translate(board_t board[static 1], point_t *start) {
    size_t offset = 0;
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
    size_t r = offset / board->width, c = offset % board->width;
    *start = (point_t){r, c};
    return true;
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
        board_set(board, pos, (char)(dir_flag | ch));
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

    point_t start;
    if (!board_translate(&board, &start)) {
        fprintf(stderr, "Start symbol (^) not found\n");
        board_delete(&board);
        return -1;
    }

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
    size_t board_size = board_length(&board);

    point_t start;
    if (!board_translate(&board, &start)) {
        fprintf(stderr, "Start symbol (^) not found\n");
        board_delete(&board);
        return -1;
    }

    board_t board_copy = board;
    board_copy.body = calloc(sizeof(char), board_size);
    if (board_copy.body == NULL) {
        perror("Failed to allocate memory for a board copy");
        board_delete(&board);
        return -1;
    }
    memcpy(board_copy.body, board.body, board_size * sizeof(char));

    traverse(&board_copy, start);
    char *orig_visits = calloc(sizeof(char), board_size);
    if (orig_visits == NULL) {
        perror("Failed to allocate memory for a board copy");
        board_delete(&board);
        board_delete(&board_copy);
        return -1;
    }
    memcpy(orig_visits, board_copy.body, board_size * sizeof(char));

    long total = 0;
    size_t start_index = start.r * board.width + start.c;
    for (size_t i = 0; i < board_size; i++) {
        if (board.body[i] != EMPTY || i == start_index
            || (orig_visits[i] & MOVE_MASK) == 0) {
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
    free(orig_visits);
    return total;
}
