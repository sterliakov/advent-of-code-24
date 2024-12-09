#include <stdlib.h>
#include <string.h>

#include "board.h"

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
    printf("============");
    for (size_t i = 0; i < board->height * board->width; i++) {
        if (i % board->width == 0)
            putc('\n', stdout);
        putc(board->body[i], stdout);
    }
    printf("\n============\n");
}

bool board_p2i(board_t board[const static 1], point_t point, size_t *index) {
    if (point.r >= board->height || point.c >= board->width) {
        return false;
    }
    *index = point.r * board->width + point.c;
    return true;
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

bool board_set(board_t board[const static 1], point_t point, char val) {
    size_t i = 0;
    if (!board_p2i(board, point, &i)) {
        return false;
    }
    board->body[i] = val;
    return true;
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
