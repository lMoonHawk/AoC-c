#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

int count_trees(char* trees, int stride, int rows, int slope_x, int slope_y) {
    int x = 0;
    int count = 0;
    for (int y = 0; y < rows; y += slope_y) {
        count += trees[y * stride + x] == '#';
        x += slope_x;
        x %= stride;
    }
    return count;
}

char* parse_input(FILE* input, int* stride) {
    char* trees = da_create(1);
    char c = fgetc(input);
    while (c != EOF) {
        if (c == '\n' && *stride == 0)
            *stride = da_length(trees);
        else if (c == '.' || c == '#')
            da_append(trees, c);
        c = fgetc(input);
    }
    return trees;
}

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    int stride = 0;
    char* trees = parse_input(input, &stride);
    fclose(input);
    int rows = da_length(trees) / stride;

    int slope_xs[] = { 1, 3, 5, 7, 1 };
    int slope_ys[] = { 1, 1, 1, 1, 2 };
    unsigned int p1 = 0;
    unsigned long p2 = 1;
    for (int i = 0; i < 5; ++i) {
        int tree_count = count_trees(trees, stride, rows, slope_xs[i], slope_ys[i]);
        p2 *= tree_count;
        if (i == 1)
            p1 = tree_count;
    }
    da_free(trees);

    printf("Part 1: %u\n", p1);
    printf("Part 2: %lu\n", p2);

}