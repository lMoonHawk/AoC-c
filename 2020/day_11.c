#include <stdio.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

typedef enum {
    ADJACENT,
    VISIBLE
} Rule;

char* parse_input(FILE* input, int* cols) {
    char* area = da_create(1);
    char c = fgetc(input);
    int nl = 0;
    while (c != EOF) {
        if (c != '\r' && c != '\n') {
            da_append(area, c);
            if (nl == 0)
                ++*cols;
        } else {
            nl = 1;
        }
        c = fgetc(input);
    }
    return area;
}

int count_occupied_adj(char* area, int rows, int cols, int i, int j) {
    int occupied = 0;
    for (int ni = i - 1; ni <= i + 1; ++ni) {
        for (int nj = j - 1; nj <= j + 1; ++nj) {
            if (ni == i && nj == j)
                continue;
            if (ni < 0 || ni >= rows || nj < 0 || nj >= cols)
                continue;
            if (area[ni * cols + nj] == '#')
                ++occupied;
        }
    }
    return occupied;
}

int count_occupied_vis(char* area, int rows, int cols, int i, int j) {
    int occupied = 0;
    for (int inc_i = -1; inc_i <= 1; ++inc_i) {
        for (int inc_j = -1; inc_j <= 1; ++inc_j) {
            if (inc_i == 0 && inc_j == 0)
                continue;
            int ni = i;
            int nj = j;
            while (1) {
                ni += inc_i;
                nj += inc_j;
                if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) {
                    break;
                } else if (area[ni * cols + nj] == '#') {
                    ++occupied;
                    break;
                } else if (area[ni * cols + nj] == 'L') {
                    break;
                }
            }
        }
    }
    return occupied;
}

int next_round(Rule rule, char* next_area, char* area, int rows, int cols) {
    int total_occupied = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (area[i * cols + j] == '.')
                continue;
            int occupied = 0, threshold = 0;
            if (rule == ADJACENT) {
                occupied = count_occupied_adj(area, rows, cols, i, j);
                threshold = 4;
            } else if (rule == VISIBLE) {
                occupied = count_occupied_vis(area, rows, cols, i, j);
                threshold = 5;
            }
            if (occupied == 0)
                next_area[i * cols + j] = '#';
            else if (occupied >= threshold)
                next_area[i * cols + j] = 'L';
            else {
                next_area[i * cols + j] = area[i * cols + j];
            }
            total_occupied += next_area[i * cols + j] == '#';
        }
    }
    return total_occupied;
}

int count_occupied(char* area_init, int rows, int cols, Rule rule) {
    int occupied = 0;
    char* area = da_copy(area_init);
    char* next_area = da_copy(area);
    do {
        occupied = next_round(rule, next_area, area, rows, cols);
        char* tmp = area;
        area = next_area;
        next_area = tmp;

    } while (memcmp(area, next_area, da_length(area)) != 0);
    da_free(area);
    da_free(next_area);
    return occupied;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");
    int cols = 0;
    char* area = parse_input(input, &cols);
    int rows = da_length(area) / cols;
    fclose(input);

    int occupied_adj = count_occupied(area, rows, cols, ADJACENT);
    int occupied_vis = count_occupied(area, rows, cols, VISIBLE);

    da_free(area);

    printf("Part 1: %d\n", occupied_adj);
    printf("Part 2: %d\n", occupied_vis);
}