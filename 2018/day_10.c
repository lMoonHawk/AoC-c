#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define _LINE_MAX 256

typedef struct {
    Point2 pos, vel;
} Point;

Point* parse_input(FILE* input) {
    Point* points = da_create(sizeof(*points));
    char buffer[_LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Point point = { 0 };
        strtok(buffer, "<");
        point.pos.x = atoi(strtok(NULL, " "));
        point.pos.y = atoi(strtok(NULL, "<"));
        point.vel.x = atoi(strtok(NULL, " "));
        point.vel.y = atoi(strtok(NULL, " "));
        da_append(points, point);
    }
    return points;
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");
    Point* points = parse_input(input);
    fclose(input);

    Point* new_points = da_copy(points);
    P2Bounds bounds = { { INT_MAX, INT_MAX }, { INT_MIN, INT_MIN } };
    P2Bounds best_bounds = bounds;
    uint64_t best_bounds_area = UINT64_MAX;
    uint64_t bounds_area = UINT64_MAX;
    int second = -1;
    while (bounds_area <= best_bounds_area) {
        ++second;
        Point* tmp = points;
        points = new_points;
        new_points = tmp;
        best_bounds = bounds;
        best_bounds_area = bounds_area;
        bounds = (P2Bounds){ { INT_MAX, INT_MAX }, { INT_MIN, INT_MIN } };
        for (size_t i = 0; i < da_length(points); ++i) {
            new_points[i].pos = p2add(points[i].pos, points[i].vel);
            if (new_points[i].pos.x < bounds.min.x) bounds.min.x = new_points[i].pos.x;
            if (new_points[i].pos.y < bounds.min.y) bounds.min.y = new_points[i].pos.y;
            if (new_points[i].pos.x > bounds.max.x) bounds.max.x = new_points[i].pos.x;
            if (new_points[i].pos.y > bounds.max.y) bounds.max.y = new_points[i].pos.y;
        }
        bounds_area = (uint64_t)(bounds.max.x - bounds.min.x + 1) * (uint64_t)(bounds.max.y - bounds.min.y + 1);
    }

    int buffer_size = best_bounds_area + best_bounds.max.y - best_bounds.min.y + 1;
    char* buffer = malloc(buffer_size);
    memset(buffer, '.', buffer_size);
    int buffer_stride = best_bounds.max.x - best_bounds.min.x + 2;
    for (size_t i = 0; i < da_length(points); ++i) {
        int index = (points[i].pos.x - best_bounds.min.x) + (points[i].pos.y - best_bounds.min.y) * buffer_stride;
        assert(index >= 0 && index < buffer_size);
        buffer[index] = '#';
    }
    for (int i = 0; i < best_bounds.max.y - best_bounds.min.y + 1; ++i)
        buffer[buffer_stride * (1 + i) - 1] = '\n';
    buffer[buffer_size - 1] = '\0';
    
    da_free(points);
    da_free(new_points);
    
    printf("Part 1: \n%s\n", buffer);
    printf("Part 2: %d\n", second);

    free(buffer);
}