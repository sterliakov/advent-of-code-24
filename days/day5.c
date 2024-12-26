#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/input.c"
#include "common.h"

typedef struct __attribute__((aligned(8))) rule_t {
    int left;
    int right;
} rule_t;
typedef struct __attribute__((aligned(16))) pair_t {
    size_t left;
    size_t right;
} pair_t;

int rules_read(FILE *input, rule_t *dest[static 1], size_t amount[static 1]);
int rule_cmp(const void *lhs, const void *rhs);

bool line_valid(
    const int line[static 1],
    size_t size,
    const rule_t rules[static 1],
    size_t rule_count,
    pair_t *mistake
);

long solve(
    FILE *input,
    long (*chk)(int[static 1], size_t, const rule_t[static 1], size_t)
) {
    rule_t *rules = NULL;
    size_t rule_count;
    if (rules_read(input, &rules, &rule_count) == -1) {
        perror("Failed to read the rules");
        return -1;
    }
    qsort(rules, rule_count, sizeof(rule_t), rule_cmp);

    size_t allocated = 8, nums_count = 0;
    int *line = malloc(allocated * sizeof(int));
    if (line == NULL) {
        perror("malloc failed");
        free(rules);
        return -1;
    }
    long total = 0;
    while (true) {
        if (!csv_int_read_line(input, ',', &line, &allocated, &nums_count)) {
            perror("Failed to read the input array");
            free(line);
            free(rules);
            return -1;
        }
        if (nums_count == 0)
            break;
        total += chk(line, nums_count, rules, rule_count);
    }

    free(rules);
    free(line);
    return total;
}

long chk1(
    int nums[static 1],
    size_t nums_count,
    const rule_t rules[static 1],
    size_t rule_count
) {
    return line_valid(nums, nums_count, rules, rule_count, NULL)
               ? nums[nums_count / 2]
               : 0;
}

long part1(FILE *input) {
    return solve(input, chk1);
}

long chk2(
    int nums[static 1],
    size_t nums_count,
    const rule_t rules[static 1],
    size_t rule_count
) {
    pair_t mistake = {0, 0};
    if (line_valid(nums, nums_count, rules, rule_count, &mistake)) {
        return 0;
    }
    do {
        int tmp = nums[mistake.left];
        nums[mistake.left] = nums[mistake.right];
        nums[mistake.right] = tmp;
    } while (!line_valid(nums, nums_count, rules, rule_count, &mistake));
    return nums[nums_count / 2];
}

long part2(FILE *input) {
    return solve(input, chk2);
}

int rules_read(FILE *input, rule_t *dest[static 1], size_t amount[static 1]) {
    size_t size = 4, curr = 0;
    rule_t *rules = malloc(size * sizeof(rule_t));
    if (rules == NULL) {
        return -1;
    }
    char nl1[2], nl2[2];
    int left, right;
    while (true) {
        int nvars = fscanf(input, "%d|%d%1[\n]%1[\n]", &left, &right, nl1, nl2);
        if (nvars < 3) {
            break;
        }
        if (curr >= size) {
            rule_t *oldptr = rules;
            size *= 2;
            rules = realloc(rules, size * sizeof(rule_t));
            if (rules == NULL) {
                free(oldptr);
                return -1;
            }
        }
        rules[curr] = (rule_t){left, right};
        curr++;
        if (nvars == 4) {
            break;
        }
    }
    *dest = rules;
    *amount = curr;
    return 0;
}

int rule_cmp(const void *lhs, const void *rhs) {
    const rule_t *const a = lhs;
    const rule_t *const b = rhs;
    int lc = ((a->left > b->left) - (a->left < b->left));
    return lc == 0 ? ((a->right > b->right) - (a->right < b->right)) : lc;
}

bool line_valid(
    const int line[static 1],
    size_t size,
    const rule_t rules[static 1],
    size_t rule_count,
    pair_t *mistake
) {
    for (const int *right = line + size - 1; right > line; right--) {
        for (const int *left = right - 1; left >= line; left--) {
            if (bsearch(
                    &(rule_t){*right, *left}, rules, rule_count, sizeof(rule_t),
                    rule_cmp
                )
                != NULL) {
                if (mistake != NULL) {
                    *mistake = (pair_t){left - line, right - line};
                }
                return false;
            }
        }
    }
    return true;
}
