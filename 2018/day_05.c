#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

size_t reduction_size(char* polymer, char ignore) {
    char* stack = da_create(sizeof(*stack));
    while (*polymer != '\0') {
        if (toupper(*polymer) != ignore) {
            if (da_length(stack) && abs(stack[da_length(stack) - 1] - *polymer) == ('a' - 'A'))
                da_pop(stack, NULL);
            else
                da_append(stack, *polymer);
        }
        ++polymer;
    }
    size_t size = da_length(stack);
    da_free(stack);
    return size;
}

int main() {
    FILE* input = fopen("data/day_05.txt", "r");

    char* polymer = da_create(sizeof(*polymer));
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r') {
        da_append(polymer, c);
        c = fgetc(input);
    }
    da_append(polymer, (char) { '\0' });
    fclose(input);

    size_t base_size = reduction_size(polymer, 0);
    size_t min_size = base_size;
    for (int i = 0; i < 26; ++i) {
        size_t size = reduction_size(polymer, 'A' + i);
        if (size < min_size) min_size = size;
    }
    da_free(polymer);

    printf("Part 1: %zu\n", base_size);
    printf("Part 2: %zu\n", min_size);
}