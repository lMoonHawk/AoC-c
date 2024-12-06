#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"


char* parse_input(FILE* input, int* stride) {
    char* table = da_create(sizeof(char));
    char c;
    *stride = 0;
    int i = 0;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n' && *stride == 0) {
            *stride = i;
        } else if (c != '\r' && c != '\n') {
            ++i;
            da_append(table, c);
        }
    }
    return table;
}

const char* xmas = "XMAS";

int main() {
    FILE* input = fopen("data/day_04.txt", "r");
    int stride;
    char* table = parse_input(input, &stride);
    fclose(input);

    int len = da_length(table);
    int count_xmas = 0;
    int count_cross_mas = 0;
    for (int k = 0; k < len; ++k) {

        int y = k / stride;
        int x = k % stride;

        if (table[k] == 'X') {
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (i == 0 && j == 0)
                        continue;
                    bool found = true;
                    for (int n = 1; n <= 3; ++n) {
                        int index = (y + n * j) * stride + (x + n * i);
                        if ((y + n * j < 0 || y + n * j >= len) ||
                            (x + n * i < 0 || x + n * i >= stride) ||
                            table[index] != xmas[n]) {
                            found = false;
                            break;
                        }
                    }
                    count_xmas += found;
                }
            }
        } else if (table[k] == 'A' && x > 0 && x < stride - 1 && y > 0 && y < len - 1) {
            int index_ul = (y - 1) * stride + x - 1;

            char ul = table[index_ul];
            char ur = table[index_ul + 2];
            char dl = table[index_ul + stride * 2];
            char dr = table[index_ul + stride * 2 + 2];

            if (((ur == 'M' && dl == 'S') || (ur == 'S' && dl == 'M')) &&
                ((ul == 'M' && dr == 'S') || (ul == 'S' && dr == 'M'))) {
                ++count_cross_mas;
            }
        }
    }

    da_free(table);
    printf("Part 1: %d\n", count_xmas);
    printf("Part 2: %d\n", count_cross_mas);
}