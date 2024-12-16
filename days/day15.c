#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

offset_t get_offset(char c) {
    switch (c) {
        case '<':
            return (offset_t){0, -1};
        case '>':
            return (offset_t){0, 1};
        case 'v':
            return (offset_t){1, 0};
        case '^':
            return (offset_t){-1, 0};
        default:
            assert(false);
    }
}

point_t find_start(board_t board[const static 1]) {
    size_t len = board_length(board);
    for (size_t i = 0; i < len; i++) {
        if (board->body[i] == '@') {
            return board_i2p(board, i);
        }
    }
    assert(false);
}

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    point_t pos = find_start(&board);

    char moves[80];
    while (fgets(moves, 80, input) != NULL) {
        for (char *c = moves; *c && *c != '\n'; c++) {
            offset_t dir = get_offset(*c);
            point_t next0 = {.r = pos.r + dir.dr, .c = pos.c + dir.dc};
            point_t next = next0;
            char c;
            while ((c = board_at(&board, next)) == 'O') {
                next = (point_t){.r = next.r + dir.dr, .c = next.c + dir.dc};
            }
            if (c == '.') {
                board_set_unchecked(&board, pos, '.');
                if (next.r != next0.r || next.c != next0.c) {
                    board_set_unchecked(&board, next, 'O');
                }
                board_set_unchecked(&board, next0, '@');
                pos = next0;
            }
        }
    }

    size_t total = 0;
    for (size_t r = 0; r < board.height; r++) {
        for (size_t c = 0; c < board.width; c++) {
            total += board_at_unchecked(&board, (point_t){r, c}) == 'O'
                         ? 100 * r + c
                         : 0;
        }
    }

    board_delete(&board);
    return (long)total;
}

bool move(board_t board[static 1], point_t start, offset_t dir, bool pair) {
    char c = board_at(board, start);
    if (c == 0 || c == '#')
        return false;
    if (c == '.')
        return true;
    point_t next = {.r = start.r + dir.dr, .c = start.c + dir.dc};
    bool ok = move(board, next, dir, false);
    if (!ok) {
        return false;
    }
    board_set_unchecked(board, next, c);
    board_set_unchecked(board, start, '.');
    if (pair || dir.dr == 0)
        return ok;
    if (c == '[')
        return move(
            board, (point_t){.r = start.r, .c = start.c + 1}, dir, true
        );
    else if (c == ']')
        return move(
            board, (point_t){.r = start.r, .c = start.c - 1}, dir, true
        );
    else
        return ok;
}

long part2(FILE *input) {
    board_t start_board = {NULL, 0, 0};
    if (!board_read(input, &start_board)) {
        perror("Failed to read the board");
        return -1;
    }

    board_t board
        = {.body = NULL,
           .height = start_board.height,
           .width = start_board.width * 2};
    board.body = calloc(board.width * board.height, sizeof(char));
    if (board.body == NULL) {
        perror("malloc failed");
        board_delete(&start_board);
        return -1;
    }
    size_t len = board_length(&board);
    for (size_t i = 0; i < len - 1; i += 2) {
        char c = start_board.body[i / 2];
        switch (c) {
            case 'O':
                board.body[i] = '[';
                board.body[i + 1] = ']';
                break;
            case '@':
                board.body[i] = '@';
                board.body[i + 1] = '.';
                break;
            default:
                board.body[i] = c;
                board.body[i + 1] = c;
        }
    }
    board_delete(&start_board);

    point_t pos = find_start(&board);

    board_t clone = {NULL, 0, 0};
    if (!board_clone(&clone, &board)) {
        perror("Failed to clone the board");
        board_delete(&board);
        return -1;
    }

    char moves[80];
    while (fgets(moves, 80, input) != NULL) {
        for (char *c = moves; *c && *c != '\n'; c++) {
            offset_t dir = get_offset(*c);
            point_t next = {.r = pos.r + dir.dr, .c = pos.c + dir.dc};
            if (move(&clone, next, dir, false)) {
                board_set_unchecked(&clone, pos, '.');
                board_set_unchecked(&clone, next, '@');
                pos = next;
                memcpy(board.body, clone.body, len * sizeof(char));
            } else {
                memcpy(clone.body, board.body, len * sizeof(char));
            }
        }
    }
    board_delete(&clone);

    size_t total = 0;
    for (size_t r = 0; r < board.height; r++) {
        for (size_t c = 0; c < board.width; c++) {
            total += board_at_unchecked(&board, (point_t){r, c}) == '['
                         ? 100 * r + c
                         : 0;
        }
    }

    board_delete(&board);
    return (long)total;
}
