#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef long long value_t;
typedef unsigned char op_t;

typedef struct __attribute__((aligned(32))) state_t {
    value_t a;
    value_t b;
    value_t c;
} state_t;

size_t read_input(FILE *input, state_t *state, op_t **ops) {
    if (fscanf(input, "Register A: %lld\n", &(*state).a) < 1
        || fscanf(input, "Register B: %lld\n", &(*state).b) < 1
        || fscanf(input, "Register C: %lld\n", &(*state).c) < 1) {
        fprintf(stderr, "Malformed input\n");
        return 0;
    }

// Just discard a decorative prefix
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(input, "\nProgram: ");
#pragma GCC diagnostic pop

    size_t count = 0;
    size_t arr_size = 2;
    *ops = malloc(arr_size * sizeof(op_t));
    if (*ops == NULL) {
        perror("Malloc failed");
        return 0;
    }
    while (true) {
        op_t x, y;
        if (fscanf(input, "%c,%c,", &x, &y) < 2) {
            break;
        }
        while (count + 1 >= arr_size) {
            arr_size *= 2;
            op_t *olddest = *ops;
            *ops = realloc(*ops, arr_size * sizeof(op_t));
            if (*ops == NULL) {
                free(olddest);
                perror("Malloc failed");
                return 0;
            }
        }
        (*ops)[count] = x - '0';
        (*ops)[count + 1] = y - '0';
        count += 2;
    }
    return count;
}

value_t opval(state_t state[const static 1], op_t operand) {
    if (operand <= 3)
        return operand;
    else if (operand == 4)
        return state->a;
    else if (operand == 5)
        return state->b;
    else if (operand == 6)
        return state->c;
    else
        return -1;
}

value_t adiv(value_t a, value_t val) {
    if (val < CHAR_BIT * sizeof(value_t)) {
        return a >> val;
    } else {
        return 0;
    }
}

bool run(
    state_t state[static 1],
    op_t ops[const static 1],
    size_t opcount,
    bool (*out)(op_t, void *),
    void *out_state
) {
    for (size_t i = 0; i < opcount; i += 2) {
        op_t op = ops[i];
        op_t operand = ops[i + 1];
        value_t val = opval(state, operand);
        switch (op) {
            case 0:
                state->a = adiv(state->a, val);
                break;
            case 1:
                state->b ^= operand;
                break;
            case 2:
                state->b = val % 8;
                break;
            case 3:
                if (state->a != 0)
                    i = val - 2;
                break;
            case 4:
                state->b ^= state->c;
                break;
            case 5:
                if (!out(val % 8, out_state))
                    return false;
                break;
            case 6:
                state->b = adiv(state->a, val);
                break;
            case 7:
                state->c = adiv(state->a, val);
                break;
            default:
                fprintf(stderr, "Unknown command: %c\n", op);
                return false;
        }
    }
    return true;
}

typedef struct __attribute__((aligned(32))) out_vec_t {
    op_t *vec;
    size_t alloc;
    size_t count;
} out_vec_t;

bool out1(op_t val, void *out_state) {
    out_vec_t *state = out_state;
    if (state->count >= state->alloc) {
        state->alloc *= 2;
        op_t *old = state->vec;
        state->vec = realloc(state->vec, state->alloc * sizeof(op_t));
        if (state->vec == NULL) {
            free(old);
            perror("realloc failed");
            return false;
        }
    }
    state->vec[state->count++] = val;
    return true;
}

long part1(FILE *input) {
    out_vec_t out_state = {NULL, 4, 0};
    out_state.vec = malloc(out_state.alloc * sizeof(op_t));
    if (out_state.vec == NULL) {
        perror("malloc failed");
        return -1;
    }

    state_t state = {0, 0, 0};
    op_t *ops = NULL;
    size_t count = read_input(input, &state, &ops);
    if (count == 0) {
        free(out_state.vec);
        free(ops);
        return -1;
    }

    long code = 0;
    if (run(&state, ops, count, out1, &out_state)) {
        assert(out_state.count > 0);
        printf("%c", out_state.vec[0] + '0');
        for (size_t i = 1; i < out_state.count; i++) {
            printf(",%c", out_state.vec[i] + '0');
        }
        putc('\n', stdout);
    } else {
        code = -1;
    }
    free(out_state.vec);
    free(ops);
    return code;
}

typedef struct __attribute__((aligned(32))) out_check_t {
    op_t *ops;
    size_t opcount;
    size_t tail;
} out_check_t;

bool out2(op_t val, void *out_state) {
    out_check_t *state = out_state;
    if (state->tail == 0 || state->ops[state->opcount - state->tail] != val % 8)
        return false;
    state->tail--;
    return true;
}

long part2(FILE *input) {
    state_t state0 = {0, 0, 0};
    op_t *ops = NULL;
    size_t count = read_input(input, &state0, &ops);
    if (count == 0) {
        free(ops);
        return -1;
    }

    size_t tail = 1;
    out_check_t out_state = {ops, count, 1};
    value_t a = 0;
    while (true) {
        out_state.tail = tail;
        state_t state = {.a = a, .b = state0.b, .c = state0.c};
        if (run(&state, ops, count, out2, &out_state) && out_state.tail == 0) {
            if (tail == count) {
                break;
            } else {
                tail++;
                a *= 8;
            }
        } else {
            a++;
        }
    }
    free(ops);
    return (long)a;
}
