#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    char pattern[10][8];
    char output[4][8];
} Entry;

Entry* parse_input(FILE* input) {
    Entry* entries = da_create(sizeof(*entries));
    Entry entry = { 0 };
    while (!feof(input)) {
        int read = 0;
        for (int i = 0; i < 10; ++i)
            read += fscanf(input, "%s", entry.pattern[i]);
        fscanf(input, " | ");
        for (int i = 0; i < 4; ++i)
            read += fscanf(input, "%s", entry.output[i]);
        if (read < 14)
            break;
        da_append(entries, entry);
    }
    return entries;
}

int count_common(char* a, char* b) {
    int freq1[7] = { 0 };
    int freq2[7] = { 0 };
    for (int i = 0; a[i] != '\0'; ++i)
        ++freq1[a[i] - 'a'];
    for (int i = 0; b[i] != '\0'; ++i)
        ++freq2[b[i] - 'a'];
    int common = 0;
    for (int i = 0; i < 7; ++i)
        common += freq1[i] > 0 && freq2[i] > 0;
    return common;
}


int main() {
    FILE* input = fopen("data/day_08.txt", "r");
    Entry* entries = parse_input(input);
    fclose(input);

    int sum_uniques = 0;
    for (size_t i = 0; i < da_length(entries); ++i) {
        for (int j = 0; j < 4; ++j) {
            int n = strlen(entries[i].output[j]);
            sum_uniques += (n == 2 || n == 3 || n == 4 || n == 7);
        }
    }
    // Digits with a unique number of segments, easily identifiable
    int unique_digits[4] = { 1, 4, 7, 8 };
    int unique_segments[10] = {
        [1] = 2,
        [4] = 4,
        [7] = 3,
        [8] = 7,
    };
    // Each other digits can be uniquely represented by the number of intersections with the "uniques"
    // digit: [#common segments with 1, #common segments with 4, ...]
    int repr_digits[6] = { 0, 2, 3, 5, 6, 9 };
    int repr_segments[10][4] = {
        [0] = { 2, 3, 3, 6 },
        [2] = { 1, 2, 2, 5 },
        [3] = { 2, 3, 3, 5 },
        [5] = { 1, 3, 2, 5 },
        [6] = { 1, 3, 2, 6 },
        [9] = { 2, 4, 3, 6 },
    };

    int sum_outputs = 0;
    for (size_t i = 0; i < da_length(entries); ++i) {
        char lookup[10][8] = { 0 };
        // Find uniques
        for (int j = 0; j < 10; ++j) {
            int len = strlen(entries[i].pattern[j]);
            for (int k = 0; k < 4; ++k) {
                if (len == unique_segments[unique_digits[k]]) {
                    strcpy(lookup[unique_digits[k]], entries[i].pattern[j]);
                    break;
                }
            }
        }
        // Find the other based on their intersection map and add to lookup
        for (int j = 0; j < 10; ++j) {
            int id[4] = { 0 };
            for (int k = 0; k < 4; ++k) {
                id[k] = count_common(lookup[unique_digits[k]], entries[i].pattern[j]);
            }
            for (int k = 0; k < 6; ++k) {
                if (memcmp(id, repr_segments[repr_digits[k]], 4 * sizeof(int)) == 0) {
                    strcpy(lookup[repr_digits[k]], entries[i].pattern[j]);
                    break;
                }
            }
        }
        int output = 0;
        for (int j = 0; j < 4; ++j) {
            int len_output = strlen(entries[i].output[j]);
            for (int k = 0; k < 10; ++k) {
                if (len_output == (int)strlen(lookup[k]) && len_output == count_common(entries[i].output[j], lookup[k])) {
                    output *= 10;
                    output += k;
                    break;
                }
            }
        }
        sum_outputs += output;
    }
    da_free(entries);

    printf("Part 1: %d\n", sum_uniques);
    printf("Part 1: %d\n", sum_outputs);
}