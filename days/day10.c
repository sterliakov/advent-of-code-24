#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "common.h"

#include "../lib/board.c"

static offset_t DIRECTIONS[] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

typedef struct __attribute__((aligned(16))) state_t {
    size_t *visit_ids;
    size_t id;
} state_t;

bool should_visit1(size_t i, void *state) {
    state_t *st = state;
    if (st->visit_ids[i] != st->id) {
        st->visit_ids[i] = st->id;
        return true;
    } else {
        return false;
    }
}

void traverse(
    const board_t board[static 1],
    int scores[static 1],
    bool (*chk)(size_t, void *),
    void *state,
    point_t start
) {
    for (size_t d = 0; d < sizeof(DIRECTIONS) / sizeof(DIRECTIONS[0]); d++) {
        point_t next = start;
        next.r += DIRECTIONS[d].dr;
        next.c += DIRECTIONS[d].dc;
        size_t i = 0;
        if (board_p2i(board, next, &i)
            && board_at(board, next) == board_at(board, start) - 1
            && chk(i, state)) {
            scores[i]++;
            traverse(board, scores, chk, state, next);
        }
    }
}

long final_score(const board_t board[static 1], int scores[static 1]) {
    long total = 0;
    for (size_t i = 0; i < board_length(board); i++) {
        if (board->body[i] == '0') {
            total += scores[i];
        }
    }
    return total;
}

long part1(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    size_t len = board_length(&board);

    int *scores = calloc(len, sizeof(int));
    if (scores == NULL) {
        perror("calloc failed");
        board_delete(&board);
        return -1;
    }

    size_t *visit_ids = calloc(len, sizeof(size_t));
    if (visit_ids == NULL) {
        perror("calloc failed");
        board_delete(&board);
        free(scores);
        return -1;
    }

    state_t state = {visit_ids, 0};
    for (size_t i = 0; i < len; i++) {
        if (board.body[i] == '9') {
            state.id = i + 1;
            traverse(
                &board, scores, should_visit1, &state, board_i2p(&board, i)
            );
        }
    }

    long total = final_score(&board, scores);

    board_delete(&board);
    free(scores);
    free(visit_ids);
    return total;
}

bool should_visit2(
    size_t __attribute__((unused)) i,
    void __attribute__((unused)) * state
) {
    return true;
}

long part2(FILE *input) {
    board_t board = {NULL, 0, 0};
    if (!board_read(input, &board)) {
        perror("Failed to read the board");
        return -1;
    }
    size_t len = board_length(&board);

    int *scores = calloc(len, sizeof(int));
    if (scores == NULL) {
        perror("calloc failed");
        board_delete(&board);
        return -1;
    }

    for (size_t i = 0; i < len; i++) {
        if (board.body[i] == '9') {
            traverse(&board, scores, should_visit2, NULL, board_i2p(&board, i));
        }
    }

    long total = final_score(&board, scores);

    board_delete(&board);
    free(scores);
    return total;
}
