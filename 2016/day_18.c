#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

bool* parse_input(FILE* input) {
    bool* row = da_create(sizeof(bool));
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r') {
        da_append(row, (bool) { c == '^' });
        c = fgetc(input);
    }
    return row;
}

int count_safe(bool* row, size_t row_size) {
    int total = 0;
    for (size_t i = 0; i < row_size; ++i) {
        total += !row[i];
    }
    return total;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");

    bool* row = parse_input(input);
    fclose(input);
    size_t row_size = da_length(row);
    bool* row_next = calloc(row_size, sizeof(bool));

    /*  trap if A & B & ~C | ~A & B & C | ~A & ~B & C | A & ~B & ~C
    <=> trap if A & ~C | ~A & C
    <=> trap if A^C */
    int total_p1;
    int total_p2 = count_safe(row, row_size);
    for (int i = 0; i < 400000 - 1; ++i) {
        if (i == 40 - 1)
            total_p1 = total_p2;

        for (size_t cell = 0; cell < row_size; ++cell) {
            bool prev = cell == 0 ? 0 : row[cell - 1];
            bool next = cell == row_size - 1 ? 0 : row[cell + 1];
            row_next[cell] = prev ^ next;
        }

        bool* tmp = row;
        row = row_next;
        row_next = tmp;
        total_p2 += count_safe(row, row_size);
    }

    printf("Part 1 : %d\n", total_p1);
    printf("Part 2 : %d\n", total_p2);

    da_free(row_next);
    free(row);
}