#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/board.c"
#include "../lib/vec.h"

typedef VEC_OF(char) str;
typedef VEC_OF(char *) str_vec;

#define MAX_PADS 26

static char numpad_c[] = "789456123 0A";
static board_t numpad = {numpad_c, 4, 3};
static char keypad_c[] = " ^A<v>";
static board_t keypad = {keypad_c, 2, 3};

inline long point_dist(const point_t a[static 1], const point_t b[static 1]) {
    return labs((long)(a->r - b->r)) + labs((long)(a->c - b->c));
}

str_vec moves(const board_t kbd[static 1], char src, char dest) {
    str_vec ans;
    VEC_WITH_CAPACITY(ans, 2);

    point_t start = board_find_first(kbd, src);
    point_t target = board_find_first(kbd, dest);
    point_t dead = board_find_first(kbd, ' ');

    bool ok1, ok2;
    if (start.r == target.r || start.c == target.c) {
        ok1 = true;
        ok2 = false;
    } else {
        ok1 = !(start.r == dead.r && target.c == dead.c);
        ok2 = !(start.c == dead.c && target.r == dead.r);
    }

    long len = point_dist(&start, &target);
    // 1 - horizontally first
    if (ok1) {
        char *s1 = calloc(len + 2, sizeof(char));
        if (s1 == NULL) {
            perror("malloc failed");
            exit(1);
        }
        size_t i = 0;
        for (size_t c = target.c; c < start.c; c++) {
            s1[i++] = '<';
        }
        for (size_t c = start.c; c < target.c; c++) {
            s1[i++] = '>';
        }
        for (size_t r = start.r; r < target.r; r++) {
            s1[i++] = 'v';
        }
        for (size_t r = target.r; r < start.r; r++) {
            s1[i++] = '^';
        }
        s1[i++] = 'A';
        VEC_PUSH(ans, s1);
    }

    // 2 - vertically first
    if (ok2) {
        char *s2 = calloc(len + 2, sizeof(char));
        if (s2 == NULL) {
            perror("malloc failed");
            exit(1);
        }
        size_t i = 0;
        for (size_t r = start.r; r < target.r; r++) {
            s2[i++] = 'v';
        }
        for (size_t r = target.r; r < start.r; r++) {
            s2[i++] = '^';
        }
        for (size_t c = target.c; c < start.c; c++) {
            s2[i++] = '<';
        }
        for (size_t c = start.c; c < target.c; c++) {
            s2[i++] = '>';
        }
        s2[i++] = 'A';
        VEC_PUSH(ans, s2);
    }

    return ans;
}

long num_from_string(const char s[static 1]) {
    while (*s && (*s <= '0' || *s > '9'))
        s++;
    return atol(s);
}

long keypad_move_cost(char src, char dest, size_t npads);

inline long keypad_code_cost(const char code[static 1], size_t npads) {
    long res = 0;
    char prev = 'A';
    for (const char *c = code; *c; c++) {
        res += keypad_move_cost(prev, *c, npads);
        prev = *c;
    }
    return res;
}

long keypad_move_cost(char src, char dest, size_t npads) {
    static long cache[5 * 5 * MAX_PADS] = {0};

    const size_t cache_key = 5UL * 5 * npads
                             + 5 * (strchr(keypad_c, src) - keypad_c - 1)
                             + (strchr(keypad_c, dest) - keypad_c - 1);
    if (cache[cache_key] > 0)
        return cache[cache_key];
    if (npads == 1) {
        point_t curr = board_find_first(&keypad, src);
        point_t target = board_find_first(&keypad, dest);
        long ans = point_dist(&curr, &target) + 1;
        cache[cache_key] = ans;
        return ans;
    }

    str_vec possible = moves(&keypad, src, dest);
    long ans = LONG_MAX;
    for (size_t i = 0; i < possible.size; i++) {
        long curr = keypad_code_cost(VEC_AT(possible, i), npads - 1);
        if (curr < ans)
            ans = curr;
        free(VEC_AT(possible, i));
    }
    VEC_DELETE(possible);

    cache[cache_key] = ans;
    return ans;
}

long numpad_move_cost(char src, char dest, size_t npads) {
    str_vec possible = moves(&numpad, src, dest);
    long ans = LONG_MAX;
    for (size_t i = 0; i < possible.size; i++) {
        long curr = keypad_code_cost(VEC_AT(possible, i), npads - 1);
        if (curr < ans)
            ans = curr;
        free(VEC_AT(possible, i));
    }
    VEC_DELETE(possible);
    return ans;
}

inline long numpad_code_cost(const char code[static 1], size_t npads) {
    long res = 0;
    char prev = 'A';
    for (const char *c = code; *c; c++) {
        res += numpad_move_cost(prev, *c, npads);
        prev = *c;
    }
    return res;
}

long part1(FILE *input) {
    char code[5] = {0};
    long ans = 0;
    while (fscanf(input, "%4s\n", code) == 1) {
        ans += numpad_code_cost(code, 3) * num_from_string(code);
    }
    return ans;
}

long part2(FILE *input) {
    char code[5] = {0};
    long ans = 0;
    while (fscanf(input, "%4s\n", code) == 1) {
        ans += numpad_code_cost(code, 26) * num_from_string(code);
    }
    return ans;
}
