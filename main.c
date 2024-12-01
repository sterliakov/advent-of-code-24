#include <stdio.h>
#include <stdlib.h>
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

int main(void) {
    FILE *input = fopen(INPUT_FILE, "r");
    if (input == NULL) {
        printf("%s", INPUT_FILE);
        perror("File not found");
        return EXIT_FAILURE;
    }

    part1(input);

    rewind(input);
    part2(input);

    fclose(input);
    return EXIT_SUCCESS;
}
