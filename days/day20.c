#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"
#include "../lib/point_queue.h"
#include "../lib/vec.h"

static offset_t neighbours[] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

const size_t EMPTY = (size_t)-1;
const long MIN_WIN = 100;

typedef VEC_OF(point_t) points_vec;

void traverse(
    const board_t board[static 1],
    size_t dist[static 1],
    point_t start,
    point_t end
) {
    point_queue_t q;
    point_queue_with_capacity(&q, 8);
    point_queue_push(&q, start);

    while (!point_queue_is_empty(&q)) {
        point_t pos;
        assert(point_queue_popfirst(&q, &pos));
        if (pos.r == end.r && pos.c == end.c) {
            break;
        }

        size_t i = board_p2i_or_panic(board, pos);
        for (size_t next_dir_i = 0; next_dir_i < 4; next_dir_i++) {
            point_t nxt = point_add(pos, neighbours[next_dir_i]);
            if (board_at(board, nxt) != '.') {
                continue;
            }
            size_t ni = board_p2i_or_panic(board, nxt);
            if (dist[ni] != EMPTY)
                continue;
            dist[ni] = dist[i] + 1;
            point_queue_push(&q, nxt);
        }
    }
    point_queue_delete(&q);
}

size_t check_pairs(
    const board_t board[static 1],
    const size_t dist[static 1],
    size_t max_dist
) {
    size_t ans = 0;
    const long ds = (long)max_dist;

    for (size_t r = 0; r < board->height; r++) {
        for (size_t c = 0; c < board->width; c++) {
            point_t pos = {r, c};
            size_t i = board_p2i_unchecked(board, pos);
            if (dist[i] == EMPTY)
                continue;
            size_t r1end
                = r + ds + 1 > board->height ? board->height : r + ds + 1;
            for (size_t r1 = r; r1 < r1end; r1++) {
                size_t rem = ds + r - r1;
                size_t c1end
                    = c + rem + 1 > board->width ? board->width : c + rem + 1;
                for (size_t c1 = r1 == r ? c + 1 : (c > rem ? c - rem : 0);
                     c1 < c1end; c1++) {
                    point_t pos1 = {r1, c1};
                    size_t i1 = board_p2i_unchecked(board, pos1);
                    if (dist[i1] == EMPTY)
                        continue;
                    long dp = labs((long)(r1 - r)) + labs((long)(c1 - c));
                    long win = labs((long)(dist[i1] - dist[i])) - dp;
                    if (win >= MIN_WIN) {
                        ans++;
                    }
                }
            }
        }
    }
    return ans;
}

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    size_t len = board_length(&board);
    size_t *dist = calloc(len, sizeof(size_t));
    if (dist == NULL) {
        board_delete(&board);
        perror("malloc failed");
        return -1;
    }
    for (size_t i = 0; i < len; i++) {
        dist[i] = EMPTY;
    }

    point_t start = board_find_first(&board, 'S');
    point_t end = board_find_first(&board, 'E');

    board_set(&board, start, '.');
    board_set(&board, end, '.');

    dist[board_p2i_unchecked(&board, start)] = 0;
    traverse(&board, dist, start, end);

    size_t ans = check_pairs(&board, dist, 2);

    board_delete(&board);
    free(dist);
    return (long)ans;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }

    size_t len = board_length(&board);
    size_t *dist = calloc(len, sizeof(size_t));
    if (dist == NULL) {
        board_delete(&board);
        perror("malloc failed");
        return -1;
    }
    for (size_t i = 0; i < len; i++) {
        dist[i] = EMPTY;
    }

    point_t start = board_find_first(&board, 'S');
    point_t end = board_find_first(&board, 'E');

    board_set(&board, start, '.');
    board_set(&board, end, '.');

    dist[board_p2i_unchecked(&board, start)] = 0;
    traverse(&board, dist, start, end);

    size_t ans = check_pairs(&board, dist, 20);

    board_delete(&board);
    free(dist);
    return (long)ans;
}
