#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define _LINE_MAX 256

Hset* parse_input(FILE* input, int* min_y, int* max_y) {
    *min_y = INT_MAX;
    *max_y = INT_MIN;
    Hset* clay = hs_create(sizeof(Point2));
    char buffer[_LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* fixed_var = strtok(buffer, "=");
        int fixed = atoi(strtok(NULL, "="));
        int lo = atoi(strtok(NULL, "."));
        int hi = atoi(strtok(NULL, "."));
        for (int k = lo; k <= hi; k++) {
            Point2 p;
            if (fixed_var[0] == 'x')
                p = (Point2){ fixed, k };
            else
                p = (Point2){ k, fixed };
            hs_insert(clay, &p);
            if (p.y < *min_y) *min_y = p.y;
            if (p.y > *max_y) *max_y = p.y;
        }
    }
    return clay;
}

int main() {
    FILE* input = fopen("data/day_17.txt", "r");

    int min_y, max_y;
    Hset* clay = parse_input(input, &min_y, &max_y);
    fclose(input);

    Hset* still_water = hs_create(sizeof(Point2));
    Hset* water = hs_create(sizeof(Point2));

    Queue q = q_create(sizeof(Point2));
    q_push(q, ((Point2){ 500, 0 }));

    while (q_length(q)) {
        Point2 p;
        q_pop(q, &p);
        if (hs_contains(*still_water, &p))
            continue;

        Point2 below = { p.x, p.y + 1 };
        while (below.y <= max_y && !hs_contains(*clay, &below) && !hs_contains(*still_water, &below)) {
            if (below.y >= min_y)
                hs_insert(water, &below);
            below = p2add(below, (Point2) { 0, 1 });
        }

        if (below.y > max_y)
            continue;

        int y = below.y - 1;
        int x_left = p.x;
        int blocked_left = 0;
        while (true) {
            hs_insert(water, &(Point2) { x_left, y });
            Point2 slide_below = (Point2){ x_left, y + 1 };
            if (!hs_contains(*clay, &slide_below) && !hs_contains(*still_water, &slide_below)) {
                q_push(q, slide_below);
                hs_insert(water, &slide_below);
                break;
            } else if (hs_contains(*clay, &(Point2) { x_left - 1, y })) {
                blocked_left = x_left;
                break;
            }
            --x_left;
        }
        int x_right = p.x;
        while (true) {
            hs_insert(water, &(Point2) { x_right, y });
            Point2 slide_below = (Point2){ x_right, y + 1 };
            if (!hs_contains(*clay, &slide_below) && !hs_contains(*still_water, &slide_below)) {
                q_push(q, slide_below);
                hs_insert(water, &slide_below);
                break;
            } else if (hs_contains(*clay, &(Point2) { x_right + 1, y })) {
                if (blocked_left) {
                    for (int i = x_left; i <= x_right; ++i)
                        hs_insert(still_water, &(Point2) { i, y });
                    q_push(q, ((Point2) { p.x, y - 1 }));
                }
                break;
            }
            ++x_right;
        }
    }

    printf("Part 1: %zu\n", hs_length(*water));
    printf("Part 2: %zu\n", hs_length(*still_water));

    hs_free(clay);
    hs_free(water);
    hs_free(still_water);
    q_free(q);
}