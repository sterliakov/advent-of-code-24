#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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
char board_at(board_t board[const static 1], point_t point);
bool board_set(board_t board[const static 1], point_t point, char val);
size_t board_length(board_t board[const static 1]);
bool board_slice(
    board_t board[const static 1],
    point_t start,
    char direction,
    size_t count,
    char *dest
);
