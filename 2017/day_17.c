#include <stdio.h>
#include <stdlib.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);

    int steps = atoi(buffer);

    int position = 0;
    int* spin = da_create(sizeof(int));
    da_append(spin, (int) { 0 });
    for (int i = 0; i < 2017; ++i) {
        position = (position + steps) % da_length(spin) + 1;
        da_insert(spin, (int) { i + 1 }, position);
    }
    int index_2017 = da_indexof(spin, (int) { 2017 });
    int value_p1 = spin[(index_2017 + 1) % da_length(spin)];
    da_free(spin);

    position = 0;
    int len_spin = 1;
    int value_p2 = 0;
    for (int i = 0; i < 50000000; ++i) {
        position = (position + steps) % len_spin++ + 1;
        if (position == 1)
            value_p2 = i + 1;
    }

    printf("Part 1: %d\n", value_p1);
    printf("Part 2: %d\n", value_p2);
}