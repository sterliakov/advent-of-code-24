#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

typedef struct __attribute__((aligned(16))) entry_t {
    int free_size;
    int size;
    int id;
} entry_t;

bool read_input(FILE *input, entry_t **entries, size_t count[static 1]) {
    size_t alloc = 4;
    *entries = malloc(alloc * sizeof(entry_t));
    if (*entries == NULL) {
        return false;
    }

    int id = 0;
    bool is_file = true;
    int c;
    while ((c = fgetc(input)) != EOF && c != '\n') {
        if (c < '0' || c > '9')
            return false;
        if (*count >= alloc) {
            alloc *= 2;
            entry_t *old = *entries;
            *entries = realloc(*entries, alloc * sizeof(entry_t));
            if (*entries == NULL) {
                free(old);
                return false;
            }
        }
        int size = c - '0';
        (*entries)[(*count)++] = (entry_t){
            is_file ? 0 : size, is_file ? size : 0, is_file ? id++ : 0};
        is_file = !is_file;
    }
    return true;
}

long part1(FILE *input) {
    long total = 0;
    entry_t *entries;
    size_t count = 0;
    if (!read_input(input, &entries, &count)) {
        free(entries);
        perror("Failed to read input");
        return -1;
    }
    long mp = 0;
    size_t last = count - 1;
    for (size_t i = 0; i < count; i++) {
        for (int s = entries[i].size; s; s--) {
            total += (mp++) * entries[i].id;
        }
        while (entries[i].free_size && last > i && last < count) {
            while (last < count && entries[last].size == 0)
                last -= 2;
            if (last < count && last > i) {
                while (entries[i].free_size > 0 && entries[last].size > 0) {
                    entries[i].size++;
                    entries[i].free_size--;
                    entries[last].size--;
                    entries[last].free_size++;
                    total += (mp++) * entries[last].id;
                }
            }
        }
    }

    free(entries);
    return total;
}

long part2(FILE *input) {
    long total = 0;
    entry_t *entries;
    size_t count = 0;
    if (!read_input(input, &entries, &count)) {
        free(entries);
        perror("Failed to read input");
        return -1;
    }

    long mp = 0;
    size_t last = count - 1;
    for (size_t i = 0; i < count; i++) {
        for (int s = entries[i].size; s; s--) {
            total += (mp++) * entries[i].id;
        }
        while (entries[i].free_size && last > i && last < count) {
            while (last < count
                   && (entries[last].size == 0
                       || entries[last].size > entries[i].free_size))
                last -= 2;
            if (last < count && last > i) {
                int moved = entries[last].size;
                for (int s = moved; s; s--) {
                    total += (mp++) * entries[last].id;
                }
                entries[i].size += moved;
                entries[i].free_size -= moved;
                entries[last].free_size = moved;
                entries[last].size = 0;
            }
        }
        last = count - 1;
        mp += entries[i].free_size;
    }

    free(entries);
    return total;
}
