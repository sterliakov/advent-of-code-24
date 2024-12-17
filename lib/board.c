#ifndef CUSTOM_BOARD_H
#define CUSTOM_BOARD_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __attribute__((aligned(16))) point_t {
    size_t r;
    size_t c;
} point_t;

typedef struct __attribute__((aligned(16))) offset_t {
    ptrdiff_t dr;
    ptrdiff_t dc;
} offset_t;

typedef struct __attribute__((aligned(32))) board_t {
    char *body;
    size_t height;
    size_t width;
} board_t;

bool board_read(FILE *input, board_t *board);
void board_print(board_t board[const static 1]);
void board_delete(board_t board[const static 1]);
bool board_clone(board_t dest[static 1], board_t src[const static 1]);
char board_at(board_t board[const static 1], point_t point);
char board_at_unchecked(board_t board[const static 1], point_t point);
bool board_set(board_t board[static 1], point_t point, char val);
void board_set_unchecked(board_t board[static 1], point_t point, char val);
bool board_p2i(board_t board[const static 1], point_t point, size_t *index);
size_t board_p2i_or_panic(board_t board[const static 1], point_t point);
size_t board_p2i_unchecked(board_t board[const static 1], point_t point);
point_t board_i2p(board_t board[const static 1], size_t index);
size_t board_length(board_t board[const static 1]);
bool board_slice(
    board_t board[const static 1],
    point_t start,
    char direction,
    size_t count,
    char *dest
);
point_t board_find_first(board_t board[const static 1], char c);

#define point_add(pos, dir) \
    ((point_t){.r = (pos).r + (dir).dr, .c = (pos).c + (dir).dc})

bool board_read(FILE *input, board_t *board) {
    size_t size = 100, read = 0;
    board->body = malloc(size * sizeof(char));
    if (board->body == NULL) {
        return false;
    }

    const size_t bufsize = 80;
    char buf[bufsize], tmp[2];
    int pos = 0;
    while (true) {
        int filled_vars = fscanf(input, "%80[^\n]%n%1[\n]", buf, &pos, tmp);
        switch (filled_vars) {
            case 2:
                if (board->width == 0) {
                    board->width = read + pos;
                }
                // Fallthrough
            case 1:
                if (read + pos >= size) {
                    size *= 2;
                    char *old_body = board->body;
                    board->body = realloc(board->body, size * sizeof(char));
                    if (board->body == NULL) {
                        free(old_body);
                        return false;
                    }
                }
                strncpy(board->body + read, buf, pos);
                read += pos;
                continue;
        }
        break;
    }
    if (board->width == 0) {
        free(board->body);
        return false;
    }
    board->height = read / board->width;
    return true;
}

void board_delete(board_t board[const static 1]) {
    free(board->body);
}

void board_print(board_t board[const static 1]) {
    assert(board->width > 0);
    for (size_t i = 0; i < board->width; i++) {
        putc('=', stdout);
    }
    for (size_t i = 0; i < board->height * board->width; i++) {
        if (i % board->width == 0)
            putc('\n', stdout);
        putc(board->body[i], stdout);
    }
    putc('\n', stdout);
    for (size_t i = 0; i < board->width; i++) {
        putc('=', stdout);
    }
    putc('\n', stdout);
}

bool board_clone(board_t dest[static 1], board_t src[const static 1]) {
    dest->width = src->width;
    dest->height = src->height;
    size_t len = board_length(src);
    dest->body = malloc(len * sizeof(char));
    if (dest->body == NULL) {
        return false;
    }
    memcpy(dest->body, src->body, len * sizeof(char));
    return true;
}

bool board_p2i(board_t board[const static 1], point_t point, size_t *index) {
    if (point.r >= board->height || point.c >= board->width) {
        return false;
    }
    *index = point.r * board->width + point.c;
    return true;
}
size_t board_p2i_or_panic(board_t board[const static 1], point_t point) {
    assert(point.r < board->height && point.c < board->width);
    return point.r * board->width + point.c;
}
size_t board_p2i_unchecked(board_t board[const static 1], point_t point) {
    return point.r * board->width + point.c;
}

point_t board_i2p(board_t board[const static 1], size_t index) {
    return (point_t){index / board->width, index % board->width};
}

char board_at(board_t board[const static 1], point_t point) {
    size_t i = 0;
    if (!board_p2i(board, point, &i)) {
        return 0;
    }
    return board->body[i];
}

char board_at_unchecked(board_t board[const static 1], point_t point) {
    return board->body[board_p2i_unchecked(board, point)];
}

bool board_set(board_t board[static 1], point_t point, char val) {
    size_t i = 0;
    if (!board_p2i(board, point, &i)) {
        return false;
    }
    board->body[i] = val;
    return true;
}

void board_set_unchecked(board_t board[static 1], point_t point, char val) {
    board->body[board_p2i_unchecked(board, point)] = val;
}

size_t board_length(board_t board[const static 1]) {
    return board->width * board->height;
}

/**
 * Extract a slice of given length from the board.
 * @param board pointer
 * @param starting point
 * @param direction - one of "ADWXQEZC"
 * @param count - amount of items to pick
 * @param dest - location to write a slice
 * @return bool - true on success, false if out of bounds
 **/
bool board_slice(
    board_t board[const static 1],
    point_t start,
    char direction,
    size_t count,
    char *dest
) {
    offset_t offset;
    switch (direction) {
        case 'A':
            offset = (offset_t){0, -1};
            break;
        case 'D':
            offset = (offset_t){0, 1};
            break;
        case 'W':
            offset = (offset_t){-1, 0};
            break;
        case 'X':
            offset = (offset_t){1, 0};
            break;
        case 'E':
            offset = (offset_t){-1, 1};
            break;
        case 'C':
            offset = (offset_t){1, 1};
            break;
        case 'Q':
            offset = (offset_t){-1, -1};
            break;
        case 'Z':
            offset = (offset_t){1, -1};
            break;
        default:
            // Unknown
            return false;
    }
    for (size_t i = 0; i < count; i++) {
        if ((dest[i] = board_at(board, start)) == 0) {
            return false;
        }
        start = (point_t){start.r + offset.dr, start.c + offset.dc};
    }
    return true;
}

point_t board_find_first(board_t board[const static 1], char c) {
    size_t len = board_length(board);
    for (size_t i = 0; i < len; i++) {
        if (board->body[i] == c) {
            return board_i2p(board, i);
        }
    }
    assert(false);
}

#endif
