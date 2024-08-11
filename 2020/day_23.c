#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

int* parse_input(FILE* input) {
    int* cups = da_create(sizeof(*cups));
    char c = fgetc(input);
    while (c != '\n' && c != '\r' && c != EOF) {
        da_append(cups, (int) { c - '0' });
        c = fgetc(input);
    }
    return cups;
}

int* move_cups(int* cups, int moves) {
    int* next = malloc(sizeof(int) * (da_length(cups) + 1));
    for (size_t i = 0; i < da_length(cups) - 1; ++i)
        next[cups[i]] = cups[i + 1];
    next[cups[da_length(cups) - 1]] = cups[0];

    int current = cups[0];
    for (int i = 0; i < moves; ++i) {
        int p1 = next[current];
        int p2 = next[p1];
        int p3 = next[p2];
        next[current] = next[p3];

        int destination = current - 1;
        while (destination < 1 || destination == p1 || destination == p2 || destination == p3) {
            --destination;
            if (destination < 1)
                destination = da_length(cups);
        }
        next[p3] = next[destination];
        next[destination] = p1;
        current = next[current];
    }
    return next;
}

int main() {
    FILE* input = fopen("data/day_23.txt", "r");
    int* cups = parse_input(input);
    fclose(input);

    int* next = move_cups(cups, 100);
    char* result = calloc(da_length(cups), 1);
    int cup = next[1];
    for (size_t i = 0; i < da_length(cups) - 1; ++i) {
        strncat(result, &(char){cup + '0'}, 1);
        cup = next[cup];
    }
    free(next);

    for (int i = da_length(cups) + 1; i <= 1000000; ++i)
        da_append(cups, i);
    next = move_cups(cups, 10000000);
    uint64_t out = (uint64_t)next[1] * next[next[1]];
    free(next);
    da_free(cups);

    printf("Part 1: %s\n", result);
    printf("Part 2: %" PRIu64 "\n", out);
    free(result);
}