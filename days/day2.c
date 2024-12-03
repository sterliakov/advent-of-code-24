#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define MAX_LINE 80

int report_is_safe(const char *line, bool can_have_error, int skip) {
    const char *line0 = line;
    int a, b, pos;
    if (skip == 0) {
        if (sscanf(line, "%d%n", &a, &pos) != 1) {
            return -1;
        }
        line += pos;
    }
    if (sscanf(line, "%d %d%n", &a, &b, &pos) != 2) {
        // If only two numbers were given, we can't delete any, but the input is
        // valid
        return skip == 0 ? 0 : -1;
    }
    int diff = 0;
    int curr_item = 1;
    do {
        line += pos;
        if (curr_item++ == skip)
            continue;
        int new_diff = b - a;
        if (new_diff == 0 || abs(new_diff) > 3 || diff * new_diff < 0) {
            if (can_have_error) {
                return report_is_safe(line0, false, curr_item - 2)
                       || report_is_safe(line0, false, curr_item - 1);
            } else {
                return 0;
            }
        }
        diff = new_diff;
        a = b;
    } while (sscanf(line, "%d%n", &b, &pos) == 1);
    return 1;
}

long solve(FILE *input, bool can_have_error) {
    char line[MAX_LINE];
    long count = 0;
    int line_nr = 0;
    while (fgets(line, MAX_LINE, input) != NULL) {
        line_nr++;
        size_t i = 0;
        for (i = 0; i < MAX_LINE; i++) {
            if (line[i] == 0)
                break;
        }
        if (i == MAX_LINE) {
            fprintf(stderr, "Max line length exceeded at line %d", line_nr);
            return -1;
        }
        int check_result = report_is_safe(line, can_have_error, -1);
        if (check_result < 0) {
            fprintf(stderr, "Invalid input at line %d", line_nr);
            return -1;
        } else if (check_result) {
            count++;
        }
    }
    return count;
}

long part1(FILE *input) {
    return solve(input, false);
}
long part2(FILE *input) {
    return solve(input, true);
}
