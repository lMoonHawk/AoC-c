#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;
typedef enum { X, Y } Axis;


typedef struct {
    int perimeter;
    int area;
    int sides;
} Region;

const int adj[4][4] = { {0, -1}, {1, 0 }, {0, 1}, {-1, 0} };
const int diag[4][4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };

char* parse_input(FILE* input, int* stride) {
    char* map = da_create(1);
    *stride = 0;
    int i = 0;
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n' && *stride == 0) {
            *stride = i;
        } else if (c != '\n' && c != '\r') {
            ++i;
            da_append(map, c);
        }
    }
    return map;
}

void visit_region(int size, char map[size][size], int y, int x, bool visited[size][size], Region* region) {
    visited[y][x] = true;
    region->area++;

    // The loop starts UP, prefill the edge check with the LEFT 
    int lx = x + adj[LEFT][X];
    int ly = y + adj[LEFT][Y];
    bool was_edge = (lx < 0 || lx >= size || ly < 0 || ly >= size || map[ly][lx] != map[y][x]);

    for (Direction i = 0; i < 4; ++i) {
        int nx = x + adj[i][X];
        int ny = y + adj[i][Y];
        if (nx < 0 || nx >= size || ny < 0 || ny >= size || map[ny][nx] != map[y][x]) {
            // 2 consecutive edge cells, outside corner
            if (was_edge)
                ++region->sides;
            ++region->perimeter;
            was_edge = true;
        } else {
            // 2 consecutive non-edge cells, check for inside corner
            if (!was_edge) {
                int dx = x + diag[i][X];
                int dy = y + diag[i][Y];
                if (map[dy][dx] != map[y][x])
                    ++region->sides;
            }
            if (!visited[ny][nx])
                visit_region(size, map, ny, nx, visited, region);
            was_edge = false;
        }
    }
}


int main() {
    FILE* input = fopen("data/day_12.txt", "r");
    int size;
    char* raw_map = parse_input(input, &size);
    fclose(input);
    char(*map)[size] = (void*)raw_map;

    bool(*visited)[size] = calloc(size * size, sizeof(bool));
    int total_cost = 0;
    int total_cost_discount = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (visited[i][j])
                continue;
            Region region = { 0 };
            visit_region(size, map, i, j, visited, &region);
            total_cost += region.area * region.perimeter;
            total_cost_discount += region.area * region.sides;
        }
    }
    free(visited);
    da_free(raw_map);

    printf("Part 1: %d\n", total_cost);
    printf("Part 2: %d\n", total_cost_discount);
}