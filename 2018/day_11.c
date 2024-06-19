#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define _LINE_MAX 256

static inline int get_power_level(int x, int y, int serial) {
    return (((x + 1 + 10) * (y + 1) + serial) * (x + 1 + 10) / 100) % 10 - 5;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");
    char buffer[_LINE_MAX] = { 0 };
    if (!fread(buffer, 1, _LINE_MAX, input)) exit(0);
    fclose(input);
    int serial = atoi(buffer);

    // Compute cumulative sum of power levels 
    int(*grid)[300] = malloc(300 * sizeof(*grid));
    grid[0][0] = get_power_level(0, 0, serial);
    for (int i = 1; i < 300; ++i) {
        grid[i][0] = get_power_level(0, i, serial) + grid[i - 1][0];
        grid[0][i] = get_power_level(i, 0, serial) + grid[0][i - 1];
    }
    for (int y = 1; y < 300; ++y)
        for (int x = 1; x < 300; ++x)
            grid[y][x] = get_power_level(x, y, serial) + grid[y - 1][x] + grid[y][x - 1] - grid[y - 1][x - 1];

    int best_x3, best_y3;
    int best_power_level3 = INT_MIN;
    int best_x, best_y, best_sz;
    int best_power_level = INT_MIN;
    for (int y = 0; y < 300; ++y) {
        for (int x = 0; x < 300; ++x) {
            int max_size = x > y ? x : y;
            for (int sz = 2; sz < 300 - max_size + 1; ++sz) {
                
                int power = grid[y + sz - 1][x + sz - 1];
                if (    x > 0) power -= grid[y + sz - 1][x - 1];
                if (    y > 0) power -= grid[y - 1][x + sz - 1];
                if (x * y > 0) power += grid[y - 1][x - 1];

                if (power > best_power_level) {
                    best_power_level = power;
                    best_x = x + 1, best_y = y + 1, best_sz = sz;
                }
                if (sz == 3 && power > best_power_level3) {
                    best_power_level3 = power;
                    best_x3 = x + 1, best_y3 = y + 1;
                }
            }
        }
    }
    free(grid);

    printf("Part 1: %d,%d\n", best_x3, best_y3);
    printf("Part 2: %d,%d,%d\n", best_x, best_y, best_sz);
}