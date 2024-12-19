#ifndef CUSTOM_POINT_QUEUE_H
#define CUSTOM_POINT_QUEUE_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "./board.c"

typedef struct __attribute__((aligned(32))) point_queue_t {
    point_t *data;
    size_t capacity;
    ptrdiff_t front;
    ptrdiff_t rear;
} point_queue_t;

void point_queue_new(point_queue_t q[static 1]);
void point_queue_with_capacity(point_queue_t q[static 1], size_t n);
inline bool point_queue_is_empty(point_queue_t q[static 1]);
inline void point_queue_push(point_queue_t q[static 1], point_t value);
inline bool
point_queue_popfirst(point_queue_t q[static 1], point_t out[static 1]);
inline bool
point_queue_poplast(point_queue_t q[static 1], point_t out[static 1]);
void point_queue_delete(point_queue_t q[static 1]);

inline void queue_squeeze(point_queue_t q[static 1]);

void point_queue_with_capacity(point_queue_t q[static 1], size_t n) {
    q->data = malloc(n * sizeof(point_t));
    if (q->data == NULL) {
        perror("Failed to malloc a queue\n");
        exit(1);
    }
    q->front = -1;
    q->rear = -1;
    q->capacity = n;
}

void point_queue_new(point_queue_t q[static 1]) {
    point_queue_with_capacity(q, 1);
}

inline bool point_queue_is_empty(point_queue_t q[static 1]) {
    return q->rear == -1;
}

inline void point_queue_push(point_queue_t q[static 1], point_t value) {
    if (q->rear >= 0 && (size_t)q->rear >= q->capacity - 1) {
        q->capacity *= 2;
        void *new_data = realloc(q->data, q->capacity * sizeof(point_t));
        if (new_data == NULL) {
            perror("Failed to realloc a queue\n");
            exit(1);
        }
        q->data = new_data;
    }
    if (q->front == -1) {
        q->front = 0;
    }
    q->data[++(q->rear)] = value;
}

inline bool
point_queue_popfirst(point_queue_t q[static 1], point_t out[static 1]) {
    if (point_queue_is_empty(q)) {
        return false;
    }
    *out = q->data[(q->front)++];
    queue_squeeze(q);
    return true;
}

inline bool
point_queue_poplast(point_queue_t q[static 1], point_t out[static 1]) {
    if (point_queue_is_empty(q)) {
        return false;
    }
    *out = q->data[q->rear];
    q->rear--;
    queue_squeeze(q);
    return true;
}

void point_queue_delete(point_queue_t q[static 1]) {
    free(q->data);
    q->rear = -1;
    q->front = -1;
    q->capacity = 0;
}

inline void queue_squeeze(point_queue_t q[static 1]) {
    if (q->front > q->rear) {
        q->front = q->rear = -1;
    } else if (q->front > 0 && (size_t)q->front > q->capacity / 2) {
        assert(q->rear > 0);
        memcpy(
            q->data, q->data + q->front,
            (q->rear - q->front + 1) * sizeof(point_t)
        );
        q->rear -= q->front;
        q->front = 0;
    }
}

#endif
