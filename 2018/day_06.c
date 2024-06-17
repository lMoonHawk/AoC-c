#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define _LINE_MAX 256

typedef struct {
    Point2 point;
    int region_size;
    bool infinite;
} Coordinate;

Coordinate* parse_input(FILE* input) {
    Coordinate* coordinates = da_create(sizeof(*coordinates));
    char buffer[_LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Point2 point = { atoi(strtok(buffer, " ")), atoi(strtok(NULL, " ")) };
        da_append(coordinates, ((Coordinate){point, 0, false }));
    }
    return coordinates;
}

int main() {
    FILE* input = fopen("data/day_06.txt", "r");
    Coordinate* coordinates = parse_input(input);
    fclose(input);

    P2Bounds bounds = (P2Bounds){ .min = {.x = INT_MAX, .y = INT_MAX }, .max = {.x = INT_MIN, .y = INT_MIN } };
    for (size_t i = 0; i < da_length(coordinates); ++i) {
        if (coordinates[i].point.x < bounds.min.x)
            bounds.min.x = coordinates[i].point.x;
        if (coordinates[i].point.y < bounds.min.y)
            bounds.min.y = coordinates[i].point.y;
        if (coordinates[i].point.x > bounds.max.x)
            bounds.max.x = coordinates[i].point.x;
        if (coordinates[i].point.y > bounds.max.y)
            bounds.max.y = coordinates[i].point.y;
    }

    int region_size = 0;
    for (int x = bounds.min.x; x <= bounds.max.x; ++x) {
        for (int y = bounds.min.y; y <= bounds.max.y; ++y) {
            int min_dist = INT_MAX;
            int min_index = 0;
            int tot_dist = 0;
            for (size_t i = 0; i < da_length(coordinates); ++i) {
                Point2 point = { x, y };
                int dist = p2dist(point, coordinates[i].point);
                tot_dist += dist;
                if (dist < min_dist) {
                    min_dist = dist;
                    min_index = i;
                } else if (dist == min_dist) {
                    min_index = -1;
                }
            }
            region_size += tot_dist < 10000;
            if (min_index >= 0) {
                ++coordinates[min_index].region_size;
                if (x == bounds.min.x || x == bounds.max.x || y == bounds.min.y || y == bounds.max.y)
                    coordinates[min_index].infinite = true;
            }
        }
    }
    int max_region_size = 0;
    for (size_t i = 0; i < da_length(coordinates); ++i) {
        if (!coordinates[i].infinite && coordinates[i].region_size > max_region_size)
            max_region_size = coordinates[i].region_size;
    }
    da_free(coordinates);

    printf("Part 1: %d\n", max_region_size);
    printf("Part 2: %d\n", region_size);
}