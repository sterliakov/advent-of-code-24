#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"

static offset_t neighbours[] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

static size_t offset_index(offset_t off) {
    for (size_t ni = 0; ni < 4; ni++) {
        offset_t n = neighbours[ni];
        if (n.dr == off.dr && n.dc == off.dc) {
            return ni;
        }
    }
    // UNREACHABLE, but assert(false) halves performance..
    return 0;
}

void traverse(
    board_t board[const static 1],
    size_t dist[static 1],
    point_t pos,
    offset_t dir,
    size_t curr,
    size_t best[static 1]
) {
    if (*best > 0 && curr >= *best)
        return;
    char c = board_at(board, pos);
    if (c != '.' && c != 'E')
        return;
    // printf("t\n");

    size_t i = board_p2i_or_panic(board, pos);
    if (dist[5 * i + 4] != 0 && dist[5 * i + 4] <= curr - 1001) {
        return;
    }

    size_t ni = offset_index(dir);
    if (dist[5 * i + ni] != 0 && dist[5 * i + ni] <= curr) {
        return;
    } else {
        dist[5 * i + ni] = curr;
        if (dist[5 * i + 4] == 0 || dist[5 * i + 4] > curr)
            dist[5 * i + 4] = curr;
        if (c == 'E') {
            *best = curr;
            return;
        }
    }

    traverse(board, dist, point_add(pos, dir), dir, curr + 1, best);
#pragma GCC unroll 4
    for (size_t next_dir_i = 0; next_dir_i < 4; next_dir_i++) {
        if (next_dir_i / 2 != ni / 2) {
            // Already went further, don't go back
            offset_t n = neighbours[next_dir_i];
            traverse(board, dist, point_add(pos, n), n, curr + 1000 + 1, best);
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

    point_t pos = board_find_first(&board, 'S');
    size_t best = 0;
    for (size_t ni = 0; ni < 4; ni++) {
        offset_t n = neighbours[ni];
        traverse(
            &board, dist, point_add(pos, n), n, 1 + (1000 * (ni != 0)), &best
        );
    }
    board_delete(&board);
    free(dist);
    return (long)best;
}

bool restore(
    board_t board[const static 1],
    size_t dist[static 1],
    bool vis[static 1],
    point_t pos,
    offset_t dir,
    size_t last
) {
    char c = board_at(board, pos);
    if (c == 'E')
        return true;
    if (c != '.')
        return false;

    size_t i = board_p2i_or_panic(board, pos);

    size_t ni = offset_index(dir);
    size_t curr = dist[5 * i + ni];
    if (curr != last + 1 && curr != last + 1001) {
        return false;
    }

    bool found_path = false;
    for (size_t next_dir_i = 0; next_dir_i < 4; next_dir_i++) {
        offset_t nn = neighbours[next_dir_i];
        found_path |= restore(board, dist, vis, point_add(pos, nn), nn, curr);
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

    point_t pos = board_find_first(&board, 'S');
    size_t best = 0;
    for (size_t ni = 0; ni < 4; ni++) {
        offset_t n = neighbours[ni];
        traverse(
            &board, dist, point_add(pos, n), n, 1 + (1000 * (ni != 0)), &best
        );
    }

    bool *vis = calloc(len, sizeof(bool));
    for (size_t ni = 0; ni < 4; ni++) {
        offset_t n = neighbours[ni];
        restore(&board, dist, vis, point_add(pos, n), n, 0);
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
