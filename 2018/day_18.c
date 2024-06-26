#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define SIDE 50
#define LINE_MAX 256

typedef enum {
    OPEN,
    TREES,
    LUMBERYARD,
    TYPES_COUNT
} Acre;

void count_neighbors(Acre* acres, int y, int x, int* neighbors) {
    for (int ny = y - 1; ny <= y + 1; ++ny) {
        for (int nx = x - 1; nx <= x + 1; ++nx) {
            if (ny == y && nx == x)
                continue;
            if (ny < 0 || ny >= SIDE || nx < 0 || nx >= SIDE)
                continue;
            ++neighbors[acres[ny * SIDE + nx]];
        }
    }
}

void next_minute(Acre** acres, Acre** next_acres, int* ressources) {
    memset(ressources, 0, sizeof(Acre) * TYPES_COUNT);
    for (int y = 0; y < SIDE; y++) {
        for (int x = 0; x < SIDE; x++) {
            Acre acre = (*acres)[y * SIDE + x];
            int neighbors[TYPES_COUNT] = { 0 };
            count_neighbors(*acres, y, x, neighbors);
            if (acre == OPEN && neighbors[TREES] >= 3)
                (*next_acres)[y * SIDE + x] = TREES;
            else if (acre == TREES && neighbors[LUMBERYARD] >= 3)
                (*next_acres)[y * SIDE + x] = LUMBERYARD;
            else if (acre == LUMBERYARD && (neighbors[LUMBERYARD] == 0 || neighbors[TREES] == 0))
                (*next_acres)[y * SIDE + x] = OPEN;
            else
                (*next_acres)[y * SIDE + x] = acre;

            ++ressources[(*next_acres)[y * SIDE + x]];
        }
    }
    Acre* tmp = *next_acres;
    *next_acres = *acres;
    *acres = tmp;
}

size_t find_cycle(int* values) {
    size_t n = da_length(values);
    for (size_t i = 2; i < n / 3; ++i)
        if (values[n - 1] == values[n - 1 - i] && values[n - 1] == values[n - 1 - i * 2])
            return i;
    return 0;
}

void* parse_input(FILE* input) {
    Acre* acres = malloc(sizeof(Acre) * SIDE * SIDE);
    char buffer[LINE_MAX] = { 0 };
    for (int i = 0; i < SIDE; i++) {
        fgets(buffer, LINE_MAX, input);
        for (int j = 0; j < SIDE; j++) {
            switch (buffer[j]) {
                case '.': acres[i * SIDE + j] = OPEN; break;
                case '|': acres[i * SIDE + j] = TREES; break;
                case '#': acres[i * SIDE + j] = LUMBERYARD; break;
            }
        }
    }
    return acres;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");
    Acre* acres = parse_input(input);
    fclose(input);

    Acre* acres_buffer = malloc(sizeof(Acre) * SIDE * SIDE);
    int ressources[TYPES_COUNT] = { 0 };
    for (int i = 0; i < SIDE * SIDE; i++)
        ++ressources[acres[i]];

    int* values = da_create(sizeof(*values));
    int cycle = 0;
    int i = 0;
    for (i = 0; i < 1000000000 && !cycle; i++) {
        next_minute(&acres, &acres_buffer, ressources);
        da_append(values, (int) { ressources[TREES] * ressources[LUMBERYARD] });
        cycle = find_cycle(values);
    }
    free(acres);
    free(acres_buffer);

    printf("Part 1: %d\n", values[9]);
    printf("Part 2: %d\n", values[(1000000000 - i - 1) % cycle + (da_length(values) - cycle)]);

    da_free(values);
}
