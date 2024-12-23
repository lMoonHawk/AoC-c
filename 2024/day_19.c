#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 4096

uint64_t count_create(char* design, char** towels) {
    int len = strcspn(design, "\r\n");
    design[len] = '\0';
    uint64_t* table = calloc(len + 1, sizeof(uint64_t));
    table[0] = 1;

    for (int i = 0; i <= len; i++) {
        if (table[i]) {
            for (size_t j = 0; j < da_length(towels); j++) {
                if (strncmp(design + i, towels[j], strlen(towels[j])) == 0) {
                    table[i + strlen(towels[j])] += table[i];
                }
            }
        }
    }
    uint64_t out = table[len];
    free(table);
    return out;
}

int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    char** towels = da_create(sizeof(char*));

    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    char* tok = strtok(buffer, ", ");
    while (tok) {
        da_append(towels, (char*) { strdup(tok) });
        tok = strtok(NULL, ", \r\n");
    }

    fgets(buffer, sizeof(buffer), input);
    int possible_count = 0;
    uint64_t combin_count = 0;

    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        uint64_t count = count_create(buffer, towels);
        if (count > 0) {
            combin_count += count;
            possible_count++;
        }
    }
    fclose(input);
    
    for (size_t i = 0; i < da_length(towels); i++) {
        free(towels[i]);
    }
    da_free(towels);

    printf("Part 1: %d\n", possible_count);
    printf("Part 2: %"  PRIu64 "\n", combin_count);
}