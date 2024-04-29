/*
To use this library, do this in *one* C or C++ file:
#define DARRAY_IMPLEMENTATION
#include "darray.h"
#define PQUEUE_IMPLEMENTATION
#include "pqueue.h"
*/

#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdlib.h>

#define pq_push(pq, val) _pq_push((pq), &(val));
#define pq_free(pq) _pq_free(&(pq));

typedef int (*pq_cmpp)(const void*, const void*);

typedef struct _Pq {
    void* heap;
    pq_cmpp cmpf;
} *Pq;

void pq_print_err(const char* msg);
void* pq_create(size_t unit_size, pq_cmpp cmpf);
size_t pq_length(Pq pq);
void pq_sift_up(Pq pq, size_t pos);
void _pq_push(Pq pq, void* value);
void pq_sift_down(Pq pq);
void* pq_peekcpy(Pq pq, void* result);
const void* pq_peek(Pq pq);
void pq_pop(Pq pq, void* result);
void _pq_free(Pq *pq);

#endif // PQUEUE_H

#ifdef PQUEUE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <assert.h>
// #include "darray.h"

void pq_print_err(const char* msg) {
    fprintf(stderr, "%s %s", "[PQ ERR]", msg);
}

void* pq_create(size_t unit_size, pq_cmpp cmpf) {
    Pq pq = malloc(sizeof(struct _Pq));
    assert(pq != NULL);
    *pq = (struct _Pq){ da_create(unit_size), cmpf };
    return pq;
}

size_t pq_length(Pq pq) {
    return da_length(pq->heap);
}

void pq_sift_up(Pq pq, size_t pos) {
    void* da = pq->heap;
    pq_cmpp cmpf = pq->cmpf;
    size_t size = da_unit_size(da);

    // Save the value
    void* value = malloc(size);
    memcpy(value, (char*)da + pos * size, size);

    size_t pos_parent = (pos - 1) / 2;

    // If the value is > its parent
    while (pos && (cmpf(value, (char*)da + pos_parent * size) > 0)) {
        // Copy the parent down
        memcpy((char*)da + pos * size, (char*)da + pos_parent * size, size);
        // Next, compare with the parent's parent.
        pos = pos_parent;
        pos_parent = (pos - 1) / 2;
    }

    // We have a final position for the value, either pos = 0 (root) or item <= parent.
    memcpy((char*)da + pos * size, value, size);
    free(value);
    return;
}

void _pq_push(Pq pq, void* value) {
    _da_append(&(pq->heap), value);
    size_t len = da_length(pq->heap);
    pq_sift_up(pq, len - 1);
}

void pq_sift_down(Pq pq) {
    void* da = pq->heap;
    size_t size = da_unit_size(da);
    size_t len = da_length(da);
    pq_cmpp cmpf = pq->cmpf;

    size_t pos = 0;
    size_t l_child_pos = 1;
    size_t r_child_pos, max_child_pos;

    // Save the value
    void* value = malloc(size);
    memcpy(value, (char*)da + pos * size, size);

    while (l_child_pos < len) {
        max_child_pos = l_child_pos;
        r_child_pos = l_child_pos + 1;
        // If right child is valid and greater than than left child
        if (r_child_pos < len && cmpf((char*)da + r_child_pos * size, (char*)da + l_child_pos * size) > 0) {
            max_child_pos = r_child_pos;
        }
        // Move child up
        memcpy((char*)da + pos * size, (char*)da + max_child_pos * size, size);
        pos = max_child_pos;
        l_child_pos = 2 * pos + 1;
    }
    // We have a semi-final position for the value.
    memcpy((char*)da + pos * size, value, size);
    free(value);
    // Sift that value up to its final position.
    pq_sift_up(pq, pos);
}

void* pq_peekcpy(Pq pq, void* result) {
    if (result == NULL) return NULL;
    void* da = pq->heap;
    return memcpy(result, (char*)da, da_unit_size(da));
}

const void* pq_peek(Pq pq) {
    return pq->heap;
}

void pq_pop(Pq pq, void* result) {
    void* da = pq->heap;
    size_t len = da_length(da);
    size_t size = da_unit_size(da);

    if (len == 0) {
        pq_print_err("Pop from an empty array\n");
        return;
    }
    // Swap first and last element of the array
    void* tmp = malloc(size);
    memcpy(tmp, (char*)da, size);
    memcpy((char*)da, (char*)da + (len - 1) * size, size);
    memcpy((char*)da + (len - 1) * size, tmp, size);
    free(tmp);

    da_pop(da, result);
    if (len > 0) pq_sift_down(pq);
}

void _pq_free(Pq *pq) {
    da_free((*pq)->heap);
    free(*pq);
    *pq = NULL;
}

#endif // PQUEUE_IMPLEMENTATION
