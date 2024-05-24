#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int index_max(int arr[16]) {
    int out = 0;
    for (int i = 1; i < 16; ++i)
        if (arr[i] > arr[out])
            out = i;
    return out;
}

void add_row(int** banks, int it) {
    for (int i = 0; i < 16; ++i) {
        int last = (*banks)[it * 16 + i];
        da_append(*banks, last);
    }
}

size_t seen(size_t it, int* banks) {
    for (size_t i = 0; i < it; ++i) {
        if (memcmp(&banks[it * 16], &banks[i * 16], 16 * sizeof(int)) == 0) {
            return it - i;
        }
    }
    return false;
}

int* parse_input(FILE* input) {
    int* banks = da_create(sizeof(*banks));

    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    da_append(banks, ((int){atoi(strtok(buffer, "\t"))}));
    for (int i = 1; i < 16; ++i)
        da_append(banks, ((int){atoi(strtok(NULL, "\t"))}));
    return banks;
}

int main() {
    FILE* input = fopen("data/day_06.txt", "r");
    int* banks = parse_input(input);
    fclose(input);

    size_t steps = 0;
    size_t cycle_len = 0;
    while (!(cycle_len = seen(steps, banks))) {
        add_row(&banks, steps++);
        int max_i = index_max(&banks[steps * 16]);
        int blocks = banks[steps * 16 + max_i];
        banks[steps * 16 + max_i] = 0;
        for (int i = 0; i < blocks; ++i) {
            banks[steps * 16 + (max_i + i + 1) % 16] += 1;
        }
    }
    da_free(banks);

    printf("Part 1: %zu\n", steps);
    printf("Part 2: %zu\n", cycle_len);
}