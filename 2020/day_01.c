#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int* parse_input(FILE* input) {
    int* entries = da_create(sizeof(int));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int entry;
        sscanf(buffer, "%d", &entry);
        da_append(entries, entry);
    }
    return entries;
}

int main() {
    FILE* input = fopen("data/day_01.txt", "r");
    int* entries = parse_input(input);
    fclose(input);

    int p1 = 0;
    int p2 = 0;
    for (size_t i = 0; i < da_length(entries); ++i) {
        for (size_t j = 0; j < da_length(entries); ++j) {
            if (entries[i] + entries[j] == 2020)
                p1 = entries[i] * entries[j];
            for (size_t k = 0; k < da_length(entries); ++k) {
                if (entries[i] + entries[j] + entries[k] == 2020)
                    p2 = entries[i] * entries[j] * entries[k];
            }
        }
    }
    da_free(entries);
    
    printf("Part 1: %d\n", p1);
    printf("Part 2: %d\n", p2);
}