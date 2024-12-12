#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "days/common.h"

#define STR(t) #t
#define STRV(t) STR(t)

#ifdef INFILE
    #ifdef DAY
        #define INPUT_FILE "inputs/day" STRV(DAY) "/" STRV(INFILE) ".txt"
    #else
        #error DAY must be defined
    #endif
#else
    #error INFILE must be defined
#endif

#ifdef TIMING
    #define MEASURE(expr)                            \
        do {                                         \
            clock_t start = clock(), diff;           \
            expr;                                    \
            diff = clock() - start;                  \
            int msec = diff * 1000 / CLOCKS_PER_SEC; \
            printf("Time taken: %d ms\n", msec);     \
        } while (0);
#else
    #define MEASURE(expr) expr;
#endif

int main(void) {
    FILE *input = fopen(INPUT_FILE, "r");
    if (input == NULL) {
        printf("%s", INPUT_FILE);
        perror("File not found");
        return EXIT_FAILURE;
    }

    MEASURE(long result_1 = part1(input); printf("Part 1: %ld\n", result_1););

    rewind(input);
    MEASURE(long result_2 = part2(input); printf("Part 2: %ld\n", result_2););

    fclose(input);
    return EXIT_SUCCESS;
}
