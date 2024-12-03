#include <stdbool.h>
#include <stdio.h>
#include "common.h"

void part1(FILE *input) {
    long total = 0;
    int a, b;
    char end[2];
    do {
        switch (fgetc(input)) {
            case EOF:
                break;
            case 'm':
                while (fscanf(input, "ul(%d,%d%1[)]", &a, &b, end) == 3) {
                    total += a * b;
                };
            default:
                continue;
        }
        break;
    } while (1);

    printf("Part 1: %ld\n", total);
}

void part2(FILE *input) {
    long total = 0;
    int a, b;
    int read_1 = -1, read_2 = -1;
    char end[2];
    bool enabled = true;
    do {
        switch (fgetc(input)) {
            case EOF:
                break;
            case 'm':
                if (enabled) {
                    while (fscanf(input, "ul(%d,%d%1[)]", &a, &b, end) == 3) {
                        total += a * b;
                    }
                }
                continue;
            case 'd':
                if (fscanf(input, "o%n()%n", &read_1, &read_2) == 0
                    && read_2 == 3) {
                    enabled = true;
                }
                read_2 = -1;
                if (read_1 == 1 && fscanf(input, "n't()%n", &read_2) == 0
                    && read_2 == 5) {
                    enabled = false;
                }
                read_1 = read_2 = -1;
                continue;
            default:
                continue;
        }
        break;
    } while (1);

    printf("Part 2: %ld\n", total);
}
