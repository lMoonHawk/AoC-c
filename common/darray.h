/*
To use this library, do this in *one* C or C++ file:
#define DARRAY_IMPLEMENTATION
#include "darray.h"

T* da = da_create(sizeof(T)) - Create a dynamic array
da_free(da)                  - Free the entire dynamic array 
da_append(da, var)           - Append var to the dynamic array da
da_pop(da, &var)             - Remove last element of dynamic array da and copies result to var (can be NULL)
da_remove(da, i)             - Remove element i from dynamic array da
da_length(da)                - Get length of dynamic array da 
da[i]                            - Access ith element of dynamic array da

TODO:
Dynamic array should shrink if length < capacity/2
*/

#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>

#define DARRAY_GROWTH 1.5
#define DARRAY_INICAP 2


enum DARRAY_HEADER {
    DARRAY_LENGTH,
    DARRAY_CAPACITY,
    DARRAY_UNIT_SIZE,
    DARRAY_FIELDS
};

#define da_append(da, val) _da_append((void**)&(da), &(val));

void print_err(const char* msg);
void* da_create(size_t unit_size);
size_t da_length(void* da);
size_t da_capacity(void* da);
size_t da_unit_size(void* da);
void da_set_length(void* da, size_t len);
void _da_append(void** da, void* value);
void da_pop(void* da, void* result);
void da_remove(void* da, size_t index);
void da_free(void* da);

#endif // DARRAY_H

#ifdef DARRAY_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <assert.h>

void print_err(const char* msg) {
    fprintf(stderr, "%s %s", "[DARRAY ERR]", msg);
}

void* da_create(size_t unit_size) {
    // Allocates for the header and initial capacity 
    size_t* dah = (size_t*)malloc(DARRAY_FIELDS * sizeof(size_t) + unit_size * DARRAY_INICAP);
    assert(dah != NULL);
    // Set the header
    dah[DARRAY_LENGTH] = 0;
    dah[DARRAY_CAPACITY] = DARRAY_INICAP;
    dah[DARRAY_UNIT_SIZE] = unit_size;
    // Returns adress after the header
    return (void*)(dah + DARRAY_FIELDS);
}

size_t da_length(void* da) {
    return *((size_t*)da - DARRAY_FIELDS + DARRAY_LENGTH);
}
size_t da_capacity(void* da) {
    return *((size_t*)da - DARRAY_FIELDS + DARRAY_CAPACITY);
}
size_t da_unit_size(void* da) {
    return *((size_t*)da - DARRAY_FIELDS + DARRAY_UNIT_SIZE);
}

void da_set_length(void* da, size_t len) {
    *((size_t*)da - DARRAY_FIELDS + DARRAY_LENGTH) = len;
}

void _da_append(void** da, void* value) {
    size_t len = da_length(*da);
    size_t capa = da_capacity(*da);
    size_t size = da_unit_size(*da);
    if (capa <= len) {
        capa *= DARRAY_GROWTH;
        size_t* dah = (size_t*)realloc((size_t*)*da - DARRAY_FIELDS, DARRAY_FIELDS * sizeof(size_t) + capa * size);
        assert(dah != NULL);
        // Set the header
        dah[DARRAY_LENGTH] = len;
        dah[DARRAY_CAPACITY] = capa;
        dah[DARRAY_UNIT_SIZE] = size;
        // Update the original array
        *da = (void*)(dah + DARRAY_FIELDS);
    }
    // Append the value
    memcpy((char*)*da + len * size, value, size);
    da_set_length(*da, ++len);
}

void da_pop(void* da, void* result) {
    size_t len = da_length(da);
    if (len <= 0) {
        print_err("Pop from an empty array\n");
    }
    else {
        size_t size = da_unit_size(da);
        char* last_element = (char*)da + (len - 1) * size;
        if (result != NULL) memcpy(result, last_element, size);
        da_set_length(da, --len);
    }
}

void da_remove(void* da, size_t index) {
    size_t len = da_length(da);
    size_t size = da_unit_size(da);

    if (index < len) {
        char* element_to_remove = (char*)da + index * size;
        char* next_element = element_to_remove + size;
        char* end_of_array = (char*)da + len * size;
        // Shift elements after index
        memmove(element_to_remove, next_element, end_of_array - next_element);
        da_set_length(da, --len);
    }
}

void da_free(void* da) {
    free((char*)da - DARRAY_FIELDS * sizeof(size_t));
}

#endif // DARRAY_IMPLEMENTATION
