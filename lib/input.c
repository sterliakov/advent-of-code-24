#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

bool csv_int_read_line(
    FILE *input,
    char sep,
    int *dest[static 1],
    size_t allocated[static 1],
    size_t *amount
) {
    size_t size = *allocated, curr = 0;
    if (size == 0) {
        return false;
    }
    int *arr = *dest;
    char comma[2];
    int a;
    char fmt[8];
    sprintf(fmt, "%%d%%1[%c]", sep);
    while (1) {
        int nvars = fscanf(input, fmt, &a, comma);
        if (nvars < 1) {
            break;
        }
        if (curr >= size) {
            int *oldptr = arr;
            size *= 2;
            arr = realloc(arr, size * sizeof(int));
            if (arr == NULL) {
                free(oldptr);
                return false;
            }
        }
        arr[curr] = a;
        curr++;
        if (nvars == 1) {
            break;
        }
    }
    *dest = arr;
    *amount = curr;
    return true;
}
