#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define SIZE 10

void* parse_input(FILE* input) {
    uint8_t* grid = malloc(sizeof(uint8_t) * SIZE * SIZE);
    char c = fgetc(input);
    int p = 0;
    while (c != EOF) {
        if (c != '\n' && c != '\r')
            grid[p++] = c - '0';
        c = fgetc(input);
    }
    return grid;
}

int flash(uint8_t grid[SIZE][SIZE]) {
    int flash_count = 0;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            ++grid[i][j];

    bool stable = false;
    while (!stable) {
        stable = true;
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (grid[i][j] > 9) {
                    stable = false;
                    ++flash_count;
                    grid[i][j] = 0;
                    for (int ni = i - 1; ni <= i + 1; ++ni) {
                        for (int nj = j - 1; nj <= j + 1; ++nj) {
                            if (ni < 0 || ni >= SIZE || nj < 0 || nj >= SIZE)
                                continue;
                            if (ni == i && nj == j)
                                continue;
                            if (grid[ni][nj] != 0)
                                ++grid[ni][nj];
                        }
                    }
                }
            }
        }
    }
    return flash_count;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");
    uint8_t(*grid)[SIZE] = parse_input(input);
    fclose(input);

    int flash_tot_100 = 0;
    int flash_step = 0;
    int step_sync = 0;
    while (flash_step != SIZE * SIZE) {
        flash_step = flash(grid);
        if(++step_sync <= 100)
            flash_tot_100 += flash_step;
    }
    free(grid);

    printf("Part 1: %d\n", flash_tot_100);
    printf("Part 2: %d\n", step_sync);
}