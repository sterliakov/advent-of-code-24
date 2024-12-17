#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include "common.h"

#include "../lib/vec.h"

typedef unsigned long long value_t;
typedef unsigned char op_t;

typedef VEC_OF(op_t) op_vec;

typedef struct __attribute__((aligned(32))) state_t {
    value_t a;
    value_t b;
    value_t c;
} state_t;

bool read_input(FILE *input, state_t *state, op_vec *ops) {
    if (fscanf(input, "Register A: %llu\n", &(*state).a) < 1
        || fscanf(input, "Register B: %llu\n", &(*state).b) < 1
        || fscanf(input, "Register C: %llu\n", &(*state).c) < 1) {
        fprintf(stderr, "Malformed input\n");
        return false;
    }

// Just discard a decorative prefix
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(input, "\nProgram: ");
#pragma GCC diagnostic pop

    op_t x, y;
    while (fscanf(input, "%c,%c,", &x, &y) == 2) {
        VEC_PUSH(*ops, x - '0');
        VEC_PUSH(*ops, y - '0');
    }
    return (*ops).size > 0;
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
    op_vec ops[const static 1],
    bool (*out)(op_t, void *),
    void *out_state
) {
    for (size_t i = 0; i < (*ops).size; i += 2) {
        op_t op = VEC_AT(*ops, i);
        op_t operand = VEC_AT(*ops, i + 1);
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

bool out1(op_t val, void *out_state) {
    op_vec *state = out_state;
    VEC_PUSH(*state, val);
    return true;
}

long part1(FILE *input) {
    state_t state = {0, 0, 0};
    op_vec ops;
    VEC_NEW(ops);
    if (!read_input(input, &state, &ops)) {
        VEC_DELETE(ops);
        return -1;
    }

    op_vec out_state;
    VEC_NEW(out_state);
    long code = 0;
    if (run(&state, &ops, out1, &out_state)) {
        assert(out_state.size > 0);
        printf("%c", VEC_AT(out_state, 0) + '0');
        for (size_t i = 1; i < out_state.size; i++) {
            printf(",%c", VEC_AT(out_state, i) + '0');
        }
        putc('\n', stdout);
    } else {
        code = -1;
    }
    VEC_DELETE(out_state);
    VEC_DELETE(ops);
    return code;
}

typedef struct __attribute__((aligned(64))) out_check_t {
    op_vec ops;
    size_t tail;
} out_check_t;

bool out2(op_t val, void *out_state) {
    out_check_t *state = out_state;
    if (state->tail == 0
        || VEC_AT(state->ops, state->ops.size - state->tail) != val % 8)
        return false;
    state->tail--;
    return true;
}

long part2(FILE *input) {
    state_t state0 = {0, 0, 0};
    op_vec ops;
    VEC_NEW(ops);
    if (!read_input(input, &state0, &ops)) {
        VEC_DELETE(ops);
        return -1;
    }

    size_t tail = 1;
    out_check_t out_state = {ops, 1};
    value_t a = 0;
    while (true) {
        out_state.tail = tail;
        state_t state = {.a = a, .b = state0.b, .c = state0.c};
        if (run(&state, &ops, out2, &out_state) && out_state.tail == 0) {
            if (tail == ops.size) {
                break;
            } else {
                tail++;
                a *= 8;
            }
        } else {
            a++;
        }
    }
    VEC_DELETE(ops);
    return (long)a;
}
