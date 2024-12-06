#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

// can't be const size_t: "VLA can't be initialized"
#define DIRECTIONS_COUNT 4

bool find_start(board_t *board, point_t *start) {
    char *i_start = strchr(board->body, '^');
    if (i_start == NULL) {
        return false;
    }
    size_t offset = (size_t)(i_start - board->body);
    size_t r = offset / board->width, c = offset % board->width;
    *start = (point_t){r, c};
    return true;
}

long traverse(board_t *board, board_t *visits, point_t pos) {
    assert(board->width == visits->width);
    assert(board->height == visits->height);

    offset_t directions[DIRECTIONS_COUNT] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    size_t dir_index = 0;

    long total = 0;
    offset_t dir = directions[dir_index];
    char dir_flag = (char)(1 << dir_index);
    char ch;
    while ((ch = board_at(board, pos)) != 0) {
        if (ch == '#') {
            pos.c -= dir.dc;
            pos.r -= dir.dr;
            dir_index = (dir_index + 1) % DIRECTIONS_COUNT;
            dir = directions[dir_index];
            dir_flag = (char)(1 << dir_index);
            continue;
        }
        char old_visit = board_at(visits, pos);
        if (old_visit == 0) {
            total++;
        } else if (old_visit & dir_flag) {
            // cycle found - been there in the same direction
            return -1;
        }
        board_set(visits, pos, (char)(dir_flag | old_visit));
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
    if (!find_start(&board, &start)) {
        fprintf(stderr, "Start symbol (^) not found\n");
        board_delete(&board);
        return -1;
    }

    board_t visits = {NULL, board.height, board.width};
    visits.body = calloc(sizeof(char), board.width * board.height);
    if (visits.body == NULL) {
        perror("Failed to allocate memory for visits");
        board_delete(&board);
        return -1;
    }

    long total = traverse(&board, &visits, start);

    board_delete(&board);
    board_delete(&visits);
    return total;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    size_t board_size = board.width * board.height;

    point_t start;
    if (!find_start(&board, &start)) {
        fprintf(stderr, "Start symbol (^) not found\n");
        board_delete(&board);
        return -1;
    }

    board_t visits = {NULL, board.height, board.width};
    visits.body = calloc(sizeof(char), board_size);
    if (visits.body == NULL) {
        perror("Failed to allocate memory for visits");
        board_delete(&board);
        return -1;
    }

    board_t board_copy = {NULL, board.height, board.width};
    board_copy.body = calloc(sizeof(char), board_size);
    if (board_copy.body == NULL) {
        perror("Failed to allocate memory for a board copy");
        board_delete(&board);
        board_delete(&visits);
        return -1;
    }

    long total = 0;
    for (size_t i = 0; i < board_size; i++) {
        if (board.body[i] != '.') {
            continue;
        }
        memcpy(board_copy.body, board.body, board_size * sizeof(char));
        board_copy.body[i] = '#';
        memset(visits.body, 0, board_size * sizeof(char));
        if (traverse(&board_copy, &visits, start) == -1)
            total++;
    }

    board_delete(&board);
    board_delete(&visits);
    board_delete(&board_copy);
    return total;
}
