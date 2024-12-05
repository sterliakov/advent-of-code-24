#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct __attribute__((aligned(16))) point {
    size_t r;
    size_t c;
} point_t;

typedef struct __attribute__((aligned(32))) board {
    char *body;
    size_t height;
    size_t width;
} board_t;

bool board_read(FILE *input, board_t *board);
void board_print(const board_t *board);
void board_delete(const board_t *board);
char board_at(const board_t *board, point_t point);
bool board_slice(
    const board_t *board,
    point_t start,
    char direction,
    size_t count,
    char *dest
);
