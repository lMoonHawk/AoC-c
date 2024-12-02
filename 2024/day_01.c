#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

void parse_input(FILE* input, int** list_left, int** list_right) {
    int n_left, n_right;
    while (fscanf(input, "%d   %d\n", &n_left, &n_right) == 2) {
        da_append(*list_left, n_left);
        da_append(*list_right, n_right);
    }
}

int intcmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int main() {
    FILE* input = fopen("data/day_01.txt", "r");

    int* list_left = da_create(sizeof(int));
    int* list_right = da_create(sizeof(int));
    parse_input(input, &list_left, &list_right);
    fclose(input);

    qsort(list_left, da_length(list_left), sizeof(int), &intcmp);
    qsort(list_right, da_length(list_right), sizeof(int), &intcmp);

    int dist_score = 0;
    int sim_score = 0;
    for (size_t i = 0; i < da_length(list_left); ++i) {
        dist_score += abs(list_left[i] - list_right[i]);
        for (size_t j = 0; j < da_length(list_right); ++j) {
            sim_score += (list_left[i] == list_right[j]) * list_left[i];
        }
    }
    da_free(list_left);
    da_free(list_right);

    printf("Part 1: %d\n", dist_score);
    printf("Part 2: %d\n", sim_score);
}