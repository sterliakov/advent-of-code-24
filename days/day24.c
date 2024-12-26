#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#include "../lib/utils.c"
#include "../lib/vec.h"

typedef struct __attribute__((aligned(8))) __attribute__((packed)) wire_t {
    char name[4];
    int_fast8_t value;
} wire_t;

typedef enum gate_type { gate_and = 1, gate_or, gate_xor } gate_type;

typedef struct __attribute__((aligned(32))) __attribute__((packed)) gate_t {
    char left[4];
    char right[4];
    char out[4];
    gate_type type;
    bool used;
} gate_t;

typedef VEC_OF(wire_t) wire_vec;
typedef VEC_OF(gate_t) gate_vec;
typedef VEC_OF(char *) str_vec;

gate_vec read_gates(FILE *input) {
    gate_vec gates;
    VEC_NEW(gates);

    gate_t gate;
    gate.used = false;
    char op[4];
    while (fscanf(
               input, " %3[^ ] %3[^ ] %3[^ ] -> %3[^\n]\n", gate.left, op,
               gate.right, gate.out
           )
           == 4) {
        switch (op[0]) {
            case 'X':
                gate.type = gate_xor;
                break;
            case 'O':
                gate.type = gate_or;
                break;
            case 'A':
                gate.type = gate_and;
                break;
            default:
                fprintf(stderr, "Malformed input %c\n", op[0]);
                exit(1);
        }
        VEC_PUSH(gates, gate);
    }
    return gates;
}
int wire_cmp(const void *left, const void *right) {
    const wire_t *a = (const wire_t *)left;
    const wire_t *b = (const wire_t *)right;
    int name_cmp = strncmp(a->name, b->name, 3);
    return name_cmp == 0 ? ((a->value < b->value) - (a->value > b->value))
                         : name_cmp;
}

int wire_name_cmp(const void *left, const void *right) {
    const wire_t *a = (const wire_t *)left;
    const wire_t *b = (const wire_t *)right;
    return strncmp(a->name, b->name, 3);
}

wire_vec dedupe(const wire_vec wires[static 1]) {
    wire_vec ans;
    VEC_WITH_CAPACITY(ans, wires->size + 1);
    wire_t prev = VEC_AT(*wires, 0);
    VEC_PUSH(ans, prev);
    for (size_t i = 1; i < wires->size; i++) {
        wire_t curr = VEC_AT(*wires, i);
        if (strncmp(curr.name, prev.name, 3) != 0) {
            VEC_PUSH(ans, curr);
        }
        prev = curr;
    }
    return ans;
}

inline wire_t *
wire_get(const wire_vec wires[static 1], const char name[static 3]) {
    wire_t *wire = bsearch(
        &(wire_t){{name[0], name[1], name[2]}, 0}, wires->data, wires->size,
        sizeof(wire_t), wire_name_cmp
    );
    if (wire == NULL) {
        fprintf(stderr, "Wire %s not found\n", name);
    }
    assert(wire != NULL);
    return wire;
}

inline int_fast8_t
gate_perform(const gate_t gate[static 1], int_fast8_t left, int_fast8_t right) {
    switch (gate->type) {
        case gate_and:
            return (int_fast8_t)(left & right);
        case gate_or:
            return (int_fast8_t)(left | right);
        case gate_xor:
            return (int_fast8_t)(left ^ right);
    }
    assert(false);
}

uint_fast64_t as_number(const wire_vec wires[static 1], char c) {
    const wire_t *first_z = insertion_point(
        &(wire_t){{c, 0, 0}, 0}, wires->data, wires->size, sizeof(wire_t),
        wire_name_cmp
    );
    const wire_t *last_z = insertion_point(
        &(wire_t){{(char)(c + 1), 0, 0}, 0}, wires->data, wires->size,
        sizeof(wire_t), wire_name_cmp
    );

    uint_fast64_t ans = 0;
    for (const wire_t *w = last_z - 1; w >= first_z; w--) {
        ans <<= 1;
        assert(w->value != -1);
        ans |= w->value;
    }

    return ans;
}

long part1(FILE *input) {
    wire_vec all_wires;
    VEC_NEW(all_wires);

    wire_t wire;
    while (fscanf(input, "%3[^\n]: %hhd", wire.name, &wire.value) == 2) {
        VEC_PUSH(all_wires, wire);
        fgetc(input);
    }

    gate_vec gates = read_gates(input);
    for (size_t i = 0; i < gates.size; i++) {
        gate_t gate = VEC_AT(gates, i);
        VEC_PUSH(
            all_wires,
            ((wire_t){{gate.left[0], gate.left[1], gate.left[2]}, -1})
        );
        VEC_PUSH(
            all_wires,
            ((wire_t){{gate.right[0], gate.right[1], gate.right[2]}, -1})
        );
        VEC_PUSH(
            all_wires, ((wire_t){{gate.out[0], gate.out[1], gate.out[2]}, -1})
        );
    }

    VEC_SORT(all_wires, wire_cmp);
    wire_vec wires = dedupe(&all_wires);
    VEC_DELETE(all_wires);

    bool all_seen;
    do {
        all_seen = true;
        for (size_t i = 0; i < gates.size; i++) {
            gate_t gate = VEC_AT(gates, i);
            if (gate.used)
                continue;
            wire_t *left = wire_get(&wires, gate.left);
            wire_t *right = wire_get(&wires, gate.right);
            if (left->value != -1 && right->value != -1) {
                wire_t *out = wire_get(&wires, gate.out);
                out->value = gate_perform(&gate, left->value, right->value);
                gate.used = true;
            } else {
                all_seen = false;
            }
        }
    } while (!all_seen);

    uint_fast64_t ans = as_number(&wires, 'z');

    VEC_DELETE(wires);
    VEC_DELETE(gates);
    return (long)ans;
}

inline void gate_out_swap(gate_t left[static 1], gate_t right[static 1]) {
    for (size_t i = 0; i < 3; i++) {
        SWAP(char, left->out[i], right->out[i]);
    }
}

inline gate_t *find_gate(
    const gate_vec gates[static 1],
    const char with[static 3],
    gate_type op,
    bool *is_left
) {
    for (size_t i = 0; i < gates->size; i++) {
        gate_t gate = VEC_AT(*gates, i);
        if (gate.type == op) {
            if (strncmp(gate.left, with, 3) == 0) {
                if (is_left != NULL)
                    *is_left = true;
                return gates->data + i;
            }
            if (strncmp(gate.right, with, 3) == 0) {
                if (is_left != NULL)
                    *is_left = false;
                return gates->data + i;
            }
        }
    }
    return NULL;
}

inline gate_t *
find_gate_by_out(const gate_vec gates[static 1], const char out[static 3]) {
    for (size_t i = 0; i < gates->size; i++) {
        gate_t gate = VEC_AT(*gates, i);
        if (strncmp(gate.out, out, 3) == 0) {
            return gates->data + i;
        }
    }
    return NULL;
}

int void_3char_cmp(const void *left, const void *right) {
    const char *const *const a = left;
    const char *const *const b = right;
    return strncmp(*a, *b, 3);
}

/**
 * Our inputs are bitwise adders following a fixed schema:

 * x00 AND y00 -> c00
 * x00 XOR y00 -> r00
 *
 * x01 AND y01 -> p01
 * x01 XOR y01 -> q01
 * q01 AND c00 -> t01
 * q01 XOR c00 -> r01
 * t01 OR p01 -> c01
 *
 * It's trivial to detect some kinds of mismatches. Fortunately, that was enough
 * for an answer. This does not detect everything (e.g. swap p01/q02 and
 c01/c02).
 **/
long part2(FILE *input) {
    wire_t wire;
    while (fscanf(input, "%3[^\n]: %hhd", wire.name, &wire.value) == 2) {
        fgetc(input);
    }
    gate_vec gates = read_gates(input);

    gate_t *carry = find_gate(&gates, "x00", gate_and, NULL);
    assert(carry != NULL);

    str_vec swaps;
    VEC_WITH_CAPACITY(swaps, 8);

    for (char b = 1; b < 99; b++) {
        char x[4] = {'x', (char)(b / 10 + '0'), (char)(b % 10 + '0')};
        char z[4] = {'z', (char)(b / 10 + '0'), (char)(b % 10 + '0')};
        gate_t *p = find_gate(&gates, x, gate_and, NULL);
        if (p == NULL)
            break;
        gate_t *q = find_gate(&gates, x, gate_xor, NULL);
        assert(q != NULL);

        bool is_left = false;
        gate_t *t = find_gate(&gates, carry->out, gate_and, &is_left);
        assert(t != NULL);
        char *got = is_left ? t->right : t->left;
        if (strncmp(got, q->out, 3) != 0) {
            gate_t *other = find_gate_by_out(&gates, got);
            VEC_PUSH(swaps, other->out);
            VEC_PUSH(swaps, q->out);
            gate_out_swap(other, q);
        }

        gate_t *r = find_gate(&gates, carry->out, gate_xor, NULL);
        assert(r != NULL);
        if (strncmp(z, r->out, 3) != 0) {
            gate_t *other = find_gate_by_out(&gates, z);
            VEC_PUSH(swaps, other->out);
            VEC_PUSH(swaps, r->out);
            gate_out_swap(other, r);
        }

        carry = find_gate(&gates, p->out, gate_or, &is_left);
        if (carry != NULL) {
            got = is_left ? carry->right : carry->left;
            if (strncmp(got, t->out, 3) != 0) {
                gate_t *other = find_gate_by_out(&gates, got);
                VEC_PUSH(swaps, other->out);
                VEC_PUSH(swaps, t->out);
                gate_out_swap(other, t);
            }
        } else {
            carry = find_gate(&gates, t->out, gate_or, &is_left);
            assert(carry != NULL);
            got = is_left ? carry->right : carry->left;
            gate_t *other = find_gate_by_out(&gates, got);
            VEC_PUSH(swaps, other->out);
            VEC_PUSH(swaps, p->out);
            gate_out_swap(other, p);
        }
    }

    VEC_SORT(swaps, void_3char_cmp);
    printf("Part 2 answer: ");
    for (size_t i = 0; i < swaps.size; i++) {
        printf("%s%c", VEC_AT(swaps, i), i == swaps.size - 1 ? '\n' : ',');
    }

    VEC_DELETE(gates);
    VEC_DELETE(swaps);
    return 0;
}
