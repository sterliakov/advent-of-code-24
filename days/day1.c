#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int cmp(const void *left, const void *right) {
    int a = *(int *)left;
    int b = *(int *)right;
    return a < b ? -1 : a > b ? +1 : 0;
}

size_t read_arrays(FILE *input, int **av, int **bv) {
    size_t count = 0;
    size_t arr_size = 2;
    int *a = malloc(arr_size * sizeof(int));
    if (a == NULL) {
        perror("Malloc failed");
        return 0;
    }
    int *b = malloc(arr_size * sizeof(int));
    if (b == NULL) {
        perror("Malloc failed");
        return 0;
    }
    while (true) {
        if (count >= arr_size) {
            a = realloc(a, arr_size * sizeof(int) * 2);
            if (a == NULL) {
                perror("Malloc failed");
                return 0;
            }
            b = realloc(b, arr_size * sizeof(int) * 2);
            if (b == NULL) {
                perror("Malloc failed");
                return 0;
            }
            arr_size *= 2;
        }
        if (fscanf(input, "%d %d", &a[count], &b[count]) < 2) {
            break;
        }
        count++;
    }
    *av = a;
    *bv = b;
    return count;
}

long part1(FILE *input) {
    int *a = NULL;
    int *b = NULL;
    size_t count = read_arrays(input, &a, &b);

    qsort(a, count, sizeof(int), cmp);
    qsort(b, count, sizeof(int), cmp);

    long sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += abs(b[i] - a[i]);
    }

    free(a);
    free(b);
    return sum;
}

long part2(FILE *input) {
    int *a = NULL;
    int *b = NULL;
    size_t count = read_arrays(input, &a, &b);

    qsort(a, count, sizeof(int), cmp);
    qsort(b, count, sizeof(int), cmp);

    size_t i = 0, j = 0;
    long sum = 0;
    while (i < count && j < count) {
        if (a[i] == b[j]) {
            int a_reps = 1, b_reps = 1;
            int elem = a[i];
            while (++i < count && a[i] == elem)
                a_reps++;
            while (++j < count && b[j] == elem)
                b_reps++;
            sum += elem * a_reps * b_reps;
        } else if (a[i] < b[j]) {
            i++;
        } else {
            j++;
        }
    }

    free(a);
    free(b);
    return sum;
}
