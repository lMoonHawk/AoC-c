/*
To use this library, do this in *one* C or C++ file:
#define DARRAY_IMPLEMENTATION
#include "darray.h"
#define QUEUE_IMPLEMENTATION
#include "queue.h"
*/

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct _Queue {
    void* arr;
    size_t head;
    size_t tail;
} *Queue;

#define q_push(q, val) _q_push((q), &(val));

void q_print_err(const char* msg);
void* q_create(size_t unit_size);
Queue q_copy(Queue q);
Queue q_slice(Queue q, int start, int end);
size_t q_length(Queue q);
void _q_resize(Queue q, size_t new_capa);
void _q_push(Queue q, void* value);
bool q_pop(Queue q, void* result);
void* q_peekcpy(Queue q, void* result);
const void* q_peek(Queue q);
void q_print(Queue q, size_t cnt, void (*q_print_item)(void*));
void q_free(Queue q);

#endif // QUEUE_H

#ifdef QUEUE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <assert.h>

void q_print_err(const char* msg) {
    fprintf(stderr, "%s %s", "[QUEUE ERR]", msg);
}

void* q_create(size_t unit_size) {
    Queue q = malloc(sizeof(struct _Queue));
    assert(q != NULL);
    *q = (struct _Queue){ .arr = da_create(unit_size) };
    return q;
}

Queue q_copy(Queue q) {
    Queue copy = malloc(sizeof(struct _Queue));
    copy->arr = da_copy(q->arr);
    copy->head = q->head;
    copy->tail = q->tail;
    return copy;
}

size_t q_length(Queue q) {
    return da_length(q->arr);
}

void _q_resize(Queue q, size_t new_capa) {
    // Queue q = *qp;
    assert(q->tail == q->head);

    // size_t capa = new_capa;
    size_t len = da_length(q->arr);
    size_t size = da_unit_size(q->arr);
    size_t after_head = len - q->head;

    _da_resize(&(q->arr), new_capa, size);

    if (q->tail == 0) {
        q->tail = after_head;
    } else {
        char* head_p = (char*)q->arr + q->head * size;
        char* new_head_p = (char*)q->arr + (new_capa - after_head) * size;
        memmove(new_head_p, head_p, after_head * size);
        q->head = new_capa - after_head;
    }
}

void _q_push(Queue q, void* value) {
    size_t size = da_unit_size(q->arr);
    size_t len = da_length(q->arr);
    size_t capa = da_capacity(q->arr);

    if (capa <= len) {
        capa *= DARRAY_GROWTH;
        _q_resize(q, capa);
    }

    memcpy((char*)q->arr + q->tail * size, value, size);
    da_set_length(q->arr, len + 1);
    q->tail++;
    if (q->tail == capa) q->tail = 0;
}

bool q_pop(Queue q, void* result) {
    void* da = q->arr;
    size_t len = da_length(da);
    size_t size = da_unit_size(da);
    size_t capa = da_capacity(q->arr);

    if (len == 0)
        return false;
    char* head = (char*)q->arr + size * q->head;
    if (result != NULL) memcpy(result, head, size);

    da_set_length(da, --len);
    q->head++;
    if (q->head == capa) q->head = 0;
    return true;
}

void* q_peekcpy(Queue q, void* result) {
    if (result == NULL) return NULL;
    size_t size = da_unit_size(q->arr);
    return memcpy(result, (char*)q->arr + q->head * size, size);
}

const void* q_peek(Queue q) {
    return (char*)q->arr + q->head * da_unit_size(q->arr);
}

Queue q_slice(Queue q, int start, int end) {
    int capa = da_capacity(q->arr);
    int el_size = da_unit_size(q->arr);
    Queue res = q_create(el_size);

    int index = q->head;
    for (int i = 0; i < start; ++i)
        index = (index + 1) % capa;
    for (int i = start; i < end; ++i) {
        char* ptr = (char*)(q->arr) + index * el_size;
        _q_push(res, ptr);
        index = (index + 1) % capa;
    }
    return res;
}

void q_print(Queue q, size_t cnt, void (*q_print_item)(void*)) {
    size_t h = q->head;
    size_t capa = da_capacity(q->arr);
    for (size_t i = 0; i < q_length(q) && i < cnt; ++i) {
        q_print_item((char*)q->arr + h * da_unit_size(q->arr));
        h = (h + 1) % capa;
    }
}

void q_free(Queue q) {
    da_free(q->arr);
    free(q);
}

#endif // QUEUE_IMPLEMENTATION