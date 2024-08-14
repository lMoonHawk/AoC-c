#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"


uint8_t* parse_input(FILE* input, int* stride) {
    uint8_t* map = da_create(sizeof(*map));
    char c;
    int x = 0;
    while ((c = fgetc(input)) != EOF) {
        if (c != '\n' && c != '\r') {
            da_append(map, (uint8_t) { c - '0' });
            ++x;
        } else if (c == '\n') {
            *stride = x;
            x = 0;
        }
    }
    return map;
}

bool is_low(Point2 point, uint8_t* map, int stride, int rows) {
    uint8_t height = map[point.y * stride + point.x];
    for (int k = 0; k < 4; ++k) {
        Point2 neigh = p2add(point, p2adjacents[k]);
        if (neigh.x < 0 || neigh.y < 0 || neigh.x >= stride || neigh.y >= rows)
            continue;
        if (map[neigh.y * stride + neigh.x] <= height)
            return false;
    }
    return true;
}

int tmp_int_cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int main() {
    FILE* input = fopen("data/day_09.txt", "r");
    int stride;
    uint8_t* map = parse_input(input, &stride);
    fclose(input);
    int rows = da_length(map) / stride;

    int risk_level = 0;
    Point2 current = { 0 };

    Hset* visited = hs_create(sizeof(Point2));
    int* basins = da_create(sizeof(*basins));

    for (current.y = 0; current.y < rows; ++current.y) {
        for (current.x = 0; current.x < stride; ++current.x) {
            int height = map[current.y * stride + current.x];
            risk_level += is_low(current, map, stride, rows) * (height + 1);
            if (hs_contains(*visited, &current) || height == 9)
                continue;
            hs_insert(visited, &current);
            Queue q = q_create(sizeof(Point2));
            q_push(q, current);
            int size = 0;
            Point2 next;
            while (q_length(q)) {
                q_pop(q, &next);
                ++size;
                for (int k = 0; k < 4; ++k) {
                    Point2 neigh = p2add(next, p2adjacents[k]);
                    if (neigh.x < 0 || neigh.y < 0 || neigh.x >= stride || neigh.y >= rows)
                        continue;
                    if (map[neigh.y * stride + neigh.x] == 9)
                        continue;
                    if (hs_contains(*visited, &neigh))
                        continue;
                    hs_insert(visited, &neigh);
                    q_push(q, neigh);
                }
            }
            da_append(basins, size);
            q_free(q);
        }
    }
    hs_free(visited);
    da_free(map);

    int largest[3] = { INT_MIN, INT_MIN, INT_MIN };
    for (size_t i = 0; i < da_length(basins); ++i) {
        if (basins[i] > largest[0]) {
            largest[2] = largest[1];
            largest[1] = largest[0];
            largest[0] = basins[i];
        } else if (basins[i] > largest[1]) {
            largest[2] = largest[1];
            largest[1] = basins[i];
        } else if (basins[i] > largest[2]) {
            largest[2] = basins[i];
        }
    }
    da_free(basins);

    printf("Part 1: %d\n", risk_level);
    printf("Part 2: %d\n", largest[0] * largest[1] * largest[2]);
}
