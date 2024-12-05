#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int csv_int_read_line(
    FILE *input,
    int *dest[static 1],
    size_t allocated[static 1],
    size_t *amount
) {
    size_t size = *allocated, curr = 0;
    if (size == 0) {
        return -1;
    }
    int *arr = *dest;
    char comma[2];
    int a;
    while (1) {
        int nvars = fscanf(input, "%d%1[,]", &a, comma);
        if (nvars < 1) {
            break;
        }
        if (curr >= size) {
            int *oldptr = arr;
            size *= 2;
            arr = realloc(arr, size * sizeof(int));
            if (arr == NULL) {
                free(oldptr);
                return -1;
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
    return 0;
}
