#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "common.h"

#include "../lib/input.c"

bool line_ok_1(long first, int *const nums, size_t nums_count, long target) {
    if (nums_count == 0)
        return first == target;
    if (first > target)
        return false;
    return line_ok_1(first * *nums, nums + 1, nums_count - 1, target)
           || line_ok_1(first + *nums, nums + 1, nums_count - 1, target);
}

long nconcat(long left, int right) {
    int r = right;
    do {
        left *= 10;
        r /= 10;
    } while (r > 0);
    return left + right;
}

bool line_ok_2(long first, int *const nums, size_t nums_count, long target) {
    if (nums_count == 0)
        return first == target;
    if (first > target)
        return false;
    return line_ok_2(first * *nums, nums + 1, nums_count - 1, target)
           || line_ok_2(first + *nums, nums + 1, nums_count - 1, target)
           || line_ok_2(
               nconcat(first, *nums), nums + 1, nums_count - 1, target
           );
}

long solve(FILE *input, bool (*chk)(long, int *const, size_t, long)) {
    size_t nums_alloc = 4, nums_count = 0;
    int *nums = malloc(nums_alloc * sizeof(int));
    if (nums == NULL) {
        perror("malloc failed");
        return -1;
    }

    long total = 0;
    long target = 0;
    for (size_t lineno = 0; fscanf(input, "%ld:", &target) == 1; lineno++) {
        if (!csv_int_read_line(input, ' ', &nums, &nums_alloc, &nums_count)) {
            perror("malloc failed");
            free(nums);
            return -1;
        }
        if (nums_count == 0) {
            fprintf(stderr, "Malformed input at line %ld\n", lineno);
            free(nums);
            return -1;
        }
        if (chk(*nums, nums + 1, nums_count - 1, target)) {
            total += target;
        }
    }
    free(nums);
    return total;
}

long part1(FILE *input) {
    return solve(input, line_ok_1);
}

long part2(FILE *input) {
    return solve(input, line_ok_2);
}
