#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"
#include "../lib/vec.h"

static offset_t neighbours[] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

typedef VEC_OF(point_t) points_vec;

void traverse(
    board_t board[const static 1],
    size_t dist[static 1],
    point_t pos,
    size_t curr,
    size_t rev_dir,
    point_t end
) {
    char c = board_at(board, pos);
    if (c != '.') {
        return;
    }

    size_t i = board_p2i_or_panic(board, pos);
    if (dist[i] != 0 && dist[i] <= curr) {
        return;
    }

    dist[i] = curr;
    if (pos.r == end.r && pos.c == end.c) {
        return;
    }

    for (size_t next_dir_i = 0; next_dir_i < 4; next_dir_i++) {
        if (rev_dir == next_dir_i)
            continue;
        point_t nxt = point_add(pos, neighbours[next_dir_i]);
        traverse(board, dist, nxt, curr + 1, next_dir_i ^ 1, end);
    }
}

points_vec read_input(FILE *input) {
    points_vec arr;
    VEC_NEW(arr);

    size_t a, b;
    while (fscanf(input, "%lu,%lu\n", &a, &b) == 2) {
        VEC_PUSH(arr, ((point_t){.r = b, .c = a}));
    }

    return arr;
}

size_t shortest_path(
    board_t *board,
    size_t *dist,
    points_vec steps,
    size_t steps_count
) {
    assert(steps.size >= steps_count);

    size_t len = board_length(board);
    memset(board->body, '.', len);
    for (size_t i = 0; i < steps_count; i++) {
        board_set(board, VEC_AT(steps, i), '#');
    }
    memset(dist, 0, len * sizeof(size_t));
    traverse(
        board, dist, (point_t){0, 0}, 0, 5,
        (point_t){board->width - 1, board->height - 1}
    );
    return dist[len - 1];
}

long part1(FILE *input) {
    // const size_t size = 6+1;
    // const size_t iters = 12;
    const size_t size = 70 + 1;
    const size_t iters = 1024;

    const size_t len = size * size;
    board_t board = {NULL, size, size};
    board.body = malloc(sizeof(char) * len);
    if (board.body == NULL) {
        perror("malloc failed");
        return -1;
    }

    points_vec steps = read_input(input);
    assert(steps.size >= iters);
    size_t *dist = malloc(len * sizeof(size_t));

    size_t ans = shortest_path(&board, dist, steps, iters);

    board_delete(&board);
    free(dist);
    VEC_DELETE(steps);
    return (long)ans;
}

long part2(FILE *input) {
    const size_t size = 70 + 1;

    const size_t len = size * size;
    board_t board = {NULL, size, size};
    board.body = malloc(sizeof(char) * len);
    if (board.body == NULL) {
        perror("malloc failed");
        return -1;
    }

    points_vec steps = read_input(input);

    size_t left = 0, right = steps.size - 1;
    size_t *dist = malloc(len * sizeof(size_t));

    while (left < right - 1) {
        size_t iters = (left + right) / 2;
        size_t path_len = shortest_path(&board, dist, steps, iters);
        if (path_len == 0) {
            // bad - no path found
            right = iters;
        } else {
            // good - found
            left = iters;
        }
    }
    // When we end, we know that `left` steps is OK, and `right` steps is
    // either bad or last entry. Assume well-formed input, don't check that.

    point_t bad_step = VEC_AT(steps, right - 1);
    printf("Part 2 answer: %lu,%lu\n", bad_step.c, bad_step.r);
    board_delete(&board);
    free(dist);
    VEC_DELETE(steps);
    return (long)right;
}
