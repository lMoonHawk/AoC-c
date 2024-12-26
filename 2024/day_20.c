#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

bool* parse_input(FILE* input, Point2* start, Point2* end, Point2* size) {
    bool* result = da_create(sizeof(bool));
    char c;
    size->x = 0;
    Point2 pos = { .x = 0, .y = 0 };
    *start = pos;
    *end = pos;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n' || c == '\r') {
            if (size->x == 0) size->x = da_length(result);
            pos.x = 0;
            ++pos.y;
        } else {
            if (c == '#') {
                da_append(result, (bool) { true });
            } else {
                da_append(result, (bool) { false });
                if (c == 'S') *start = pos;
                else if (c == 'E') *end = pos;
            }
            pos.x++;
        }
    }
    size->y = da_length(result) / size->x;
    return result;
}

int main() {
    FILE* input = fopen("data/day_20.txt", "r");
    Point2 size;
    Point2 start, end;
    bool* maze = parse_input(input, &start, &end, &size);
    P2Bounds bounds = { .min = {.x = 0, .y = 0 }, .max = {.x = size.x - 1, .y = size.y - 1 } };
    fclose(input);

    Point2* path = da_create(sizeof(*path));
    da_append(path, start);
    while (!p2eq(path[da_length(path) - 1], end)) {
        for (P2Direction dir = 0; dir < P2_ADJCOUNT; ++dir) {
            Point2 next = p2add(path[da_length(path) - 1], p2adjacents[dir]);
            if (p2in_bounds(next, bounds) && !maze[next.y * size.x + next.x]) {
                if (da_length(path) == 1 || !p2eq(next, path[da_length(path) - 2])) {
                    da_append(path, next);
                    break;
                }
            }
        }
    }
    da_free(maze);

    int count_2ps = 0;
    int count_20ps = 0;
    for (size_t i = 0; i < da_length(path); ++i) {
        for (size_t j = i + 1; j < da_length(path); ++j) {
            int dist = p2dist(path[i], path[j]);
            if (j - i - dist >= 100) {
                count_2ps += dist <= 2;
                count_20ps += dist <= 20;
            }
        }
    }
    da_free(path);

    printf("Part 1:%d\n", count_2ps);
    printf("Part 2:%d\n", count_20ps);
}