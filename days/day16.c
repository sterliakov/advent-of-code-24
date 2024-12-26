#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

static offset_t neighbours[] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

void traverse(
    const board_t board[static 1],
    size_t dist[static 1],
    point_t pos,
    unsigned char ndir,
    size_t curr,
    size_t best[static 1]
) {
    if (*best > 0 && curr >= *best)
        return;
    char c = board_at(board, pos);
    if (c == 'E') {
        *best = curr;
    }
    if (c != '.')
        return;

    size_t i = board_p2i_or_panic(board, pos);
    if (dist[5 * i + 4] != 0 && dist[5 * i + 4] <= curr - 1001) {
        return;
    }

    if (dist[5 * i + ndir] != 0 && dist[5 * i + ndir] <= curr) {
        return;
    } else {
        dist[5 * i + ndir] = curr;
        if (dist[5 * i + 4] == 0 || dist[5 * i + 4] > curr)
            dist[5 * i + 4] = curr;
    }

    offset_t dir = neighbours[ndir];
    traverse(board, dist, point_add(pos, dir), ndir, curr + 1, best);
#pragma GCC unroll 4
    for (size_t next_dir_i = 0; next_dir_i < 4; next_dir_i++) {
        point_t nxt = point_add(pos, neighbours[next_dir_i]);
        if (next_dir_i / 2 != ndir / 2 && board_at(board, nxt) != '#') {
            // Already went further, don't go back
            traverse(board, dist, nxt, next_dir_i, curr + 1000 + 1, best);
        }
    }
}

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    size_t *dist = calloc(board.width * board.height * 5, sizeof(size_t));
    if (dist == NULL) {
        board_delete(&board);
        perror("malloc failed");
        return -1;
    }

    point_t pos = board_find_first(&board, 'S');
    size_t best = 0;
    for (unsigned char ni = 0; ni < 4; ni++) {
        traverse(
            &board, dist, point_add(pos, neighbours[ni]), ni,
            1 + (1000 * (ni != 0)), &best
        );
    }
    board_delete(&board);
    free(dist);
    return (long)best;
}

bool restore(
    const board_t board[static 1],
    size_t dist[static 1],
    bool vis[static 1],
    point_t pos,
    unsigned char ndir,
    size_t last
) {
    char c = board_at(board, pos);
    if (c == 'E')
        return true;
    if (c != '.')
        return false;

    size_t i = board_p2i_or_panic(board, pos);

    size_t curr = dist[5 * i + ndir];
    if (curr != last + 1 && curr != last + 1001) {
        return false;
    }

    bool found_path = false;
    for (size_t next_dir_i = 0; next_dir_i < 4; next_dir_i++) {
        offset_t nn = neighbours[next_dir_i];
        found_path
            |= restore(board, dist, vis, point_add(pos, nn), next_dir_i, curr);
    }
    if (found_path) {
        vis[i] = true;
    }
    return found_path;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    size_t len = board_length(&board);
    size_t *dist = calloc(len * 5, sizeof(size_t));
    if (dist == NULL) {
        board_delete(&board);
        perror("malloc failed");
        return -1;
    }

    point_t pos = board_find_first(&board, 'S');
    size_t best = 0;
    for (unsigned char ni = 0; ni < 4; ni++) {
        traverse(
            &board, dist, point_add(pos, neighbours[ni]), ni,
            1 + (1000 * (ni != 0)), &best
        );
    }

    bool *vis = calloc(len, sizeof(bool));
    for (size_t ni = 0; ni < 4; ni++) {
        restore(&board, dist, vis, point_add(pos, neighbours[ni]), ni, 0);
    }
    free(dist);
    board_delete(&board);

    long total = 0;
    for (size_t i = 0; i < len; i++) {
        total += vis[i] == true;
    }
    free(vis);
    return total + 2;
}
