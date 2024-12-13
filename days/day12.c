#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

typedef void (*stepper_t)(
    board_t[static 1],
    size_t[static 1],
    char,
    point_t,
    size_t[static 1],
    size_t[static 1],
    size_t
);

long traverse(
    board_t board[static 1],
    size_t visited[static 1],
    stepper_t step
) {
    size_t total = 0;
    size_t id = 1;
    for (size_t r = 0; r < board->height; r++) {
        for (size_t c = 0; c < board->width; c++) {
            point_t pos = {r, c};
            if (visited[board_p2i_or_panic(board, pos)] > 0)
                continue;
            size_t a = 0, p = 0;
            step(board, visited, board_at(board, pos), pos, &a, &p, id);
            total += a * p;
            id++;
        }
    }
    return (long)total;
}

void traverse_helper(
    board_t board[static 1],
    size_t visited[static 1],
    char target,
    point_t pos,
    size_t a[static 1],
    size_t p[static 1],
    size_t id
) {
    visited[board_p2i_or_panic(board, pos)] = id;
    (*a)++;
    static offset_t neighbours[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (size_t i = 0; i < sizeof(neighbours) / sizeof(neighbours[0]); i++) {
        point_t next = {pos.r + neighbours[i].dr, pos.c + neighbours[i].dc};
        if (board_at(board, next) != target) {
            (*p)++;
        } else if (visited[board_p2i_or_panic(board, next)] != id) {
            traverse_helper(board, visited, target, next, a, p, id);
        }
    }
}

void traverse_helper2(
    board_t board[static 1],
    size_t visited[static 1],
    char target,
    point_t pos,
    size_t area[static 1],
    size_t perimeter[static 1],
    size_t id
) {
    visited[board_p2i_or_panic(board, pos)] = id;
    (*area)++;

    static offset_t corner_checks[] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (size_t i = 0; i < sizeof(corner_checks) / sizeof(corner_checks[0]);
         i++) {
        offset_t checks = corner_checks[i];
        char a = board_at(board, (point_t){.r = pos.r + checks.dr, .c = pos.c});
        char b = board_at(board, (point_t){.r = pos.r, .c = pos.c + checks.dc});
        if (a != target && b != target) {
            (*perimeter)++;
        } else if (a == target && b == target) {
            char c = board_at(
                board, (point_t){.r = pos.r + checks.dr, .c = pos.c + checks.dc}
            );
            (*perimeter) += c != target;
        }
    }

    static offset_t neighbours[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (size_t i = 0; i < sizeof(neighbours) / sizeof(neighbours[0]); i++) {
        point_t next = {pos.r + neighbours[i].dr, pos.c + neighbours[i].dc};
        if (board_at(board, next) == target
            && visited[board_p2i_or_panic(board, next)] != id) {
            traverse_helper2(board, visited, target, next, area, perimeter, id);
        }
    }
}

long solve(FILE *input, stepper_t step) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    size_t *visited = calloc(board_length(&board), sizeof(size_t));
    if (visited == NULL) {
        perror("Failed to allocate visited");
        board_delete(&board);
        return -1;
    }

    long total = traverse(&board, visited, step);

    board_delete(&board);
    free(visited);
    return total;
}

long part1(FILE *input) {
    return solve(input, traverse_helper);
}
long part2(FILE *input) {
    return solve(input, traverse_helper2);
}
