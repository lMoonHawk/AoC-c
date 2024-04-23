#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

const ssize_t rows = 100;
const ssize_t stride = 100;

void swap_grid(bool** g1, bool** g2) {
    bool* tmp = *g1;
    *g1 = *g2;
    *g2 = tmp;
}

int cnt_neigh(bool* grid, ssize_t i, ssize_t j) {
    int neigh = 0;
    for (ssize_t ni = i - 1; ni <= i + 1; ++ni) {
        for (ssize_t nj = j - 1; nj <= j + 1; ++nj) {
            if (ni == i && nj == j) continue;
            if (ni < 0 || ni >= rows || nj < 0 || nj >= stride) continue;
            neigh += grid[ni * stride + nj];
        }
    }
    return neigh;
}

int next_state(bool* grid, bool** grid_next, bool stuck) {
    int lights = 0;
    for (ssize_t i = 0; i < rows; ++i) {
        for (ssize_t j = 0; j < stride; ++j) {
            ssize_t idx = i * stride + j;
            bool cell = grid[idx];
            int neigh = cnt_neigh(grid, i, j);

            (*grid_next)[idx] = cell;
            if ((cell == 1 && neigh != 2 && neigh != 3) || (cell == 0 && neigh == 3)) {
                (*grid_next)[idx] = 1 - cell;
            }
            if (stuck && (i == 0 || i == rows - 1) && (j == 0 || j == stride - 1)) {
                (*grid_next)[idx] = 1;
            }
            lights += (*grid_next)[idx];
        }
    }
    return lights;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");
    bool* grid_p1 = malloc(rows * stride * sizeof(bool));
    bool* grid_p2 = malloc(rows * stride * sizeof(bool));
    bool* grid_next = malloc(rows * stride * sizeof(bool));

    char c;
    size_t i = 0;
    while ((c = fgetc(input)) != EOF) if (c != '\n') grid_p1[i++] = c == '#';
    memcpy(grid_p2, grid_p1, rows * stride * sizeof(bool));
    grid_p2[0 * stride + 99] = true; 
    grid_p2[0 * stride + 99] = true;
    grid_p2[99 * stride + 0] = true;
    grid_p2[99 * stride + 0] = true;

    int lights_p1;
    int lights_p2;
    for (int i = 0; i < 100; ++i) {
        lights_p1 = next_state(grid_p1, &grid_next, false);
        swap_grid(&grid_p1, &grid_next);
        lights_p2 = next_state(grid_p2, &grid_next, true);
        swap_grid(&grid_p2, &grid_next);
    }

    printf("Part 1: %d\n", lights_p1);
    printf("Part 2: %d", lights_p2);

    free(grid_p1);
    free(grid_p2);
    free(grid_next);
    fclose(input);
}