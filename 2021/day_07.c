#include <stdio.h>
#include <limits.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

int* parse_input(FILE* input) {
    int* positions = da_create(sizeof(int));

    int buffer = 0;
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r') {
        if (c == ',') {
            da_append(positions, buffer);
            buffer = 0;
        } else {
            buffer = buffer * 10 + (c - '0');
        }
        c = fgetc(input);
    }
    da_append(positions, buffer);
    return positions;
}

int inc_cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");
    int* positions = parse_input(input);
    fclose(input);
    int n = da_length(positions);
    qsort(positions, n, sizeof(int), inc_cmp);

    int median = positions[n / 2];
    int fuel_spent = 0;
    int lo_coord = 0;
    for (size_t i = 0; i < da_length(positions); ++i) {
        fuel_spent += abs(positions[i] - median);
        lo_coord += positions[i];
    }
    lo_coord /= n;
    int hi_coord = lo_coord;
    int min_fuel = INT_MAX;
    while (1) {
        int lo_fuel = 0;
        int hi_fuel = 0;
        for (int i = 0; i < n; ++i) {
            lo_fuel += (1 + abs(positions[i] - lo_coord)) * abs(positions[i] - lo_coord) / 2;
            hi_fuel += (1 + abs(positions[i] - hi_coord)) * abs(positions[i] - hi_coord) / 2;
        }
        int current_min = lo_fuel < hi_fuel ? lo_fuel : hi_fuel;
        min_fuel = current_min < min_fuel ? current_min : min_fuel;

        if (min_fuel < lo_fuel && min_fuel < hi_fuel)
            break;
        --lo_coord;
        ++hi_coord;
    }
    da_free(positions);

    printf("Part 1: %d\n", fuel_spent);
    printf("Part 2: %d\n", min_fuel);
}