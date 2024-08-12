#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define _LINE_MAX 256

Seg2* parse_input(FILE* input, P2Bounds* bounds) {
    Seg2* segments = da_create(sizeof(*segments));
    char buffer[_LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Seg2 segment;
        sscanf(buffer, "%d,%d -> %d,%d", &segment.start.x, &segment.start.y, &segment.end.x, &segment.end.y);
        p2bounds_update(bounds, segment.start);
        p2bounds_update(bounds, segment.end);
        da_append(segments, segment);
    }
    return segments;
}

int max(int a, int b) {
    return (a >= b) ? a : b;
}

int main() {
    FILE* input = fopen("data/day_05.txt", "r");
    P2Bounds bounds = p2bounds_max();
    Seg2* segments = parse_input(input, &bounds);
    fclose(input);


    int (*orth_intersections)[bounds.max.x + 1] = calloc(bounds.max.y + 1, sizeof(*orth_intersections));
    int (*intersections)[bounds.max.x + 1] = calloc(bounds.max.y + 1, sizeof(*intersections));
    int orth_overlap_count = 0;
    int overlap_count = 0;

    for (size_t i = 0; i < da_length(segments); i++) {
        Point2 start = segments[i].start;
        Point2 end = segments[i].end;
        bool diag = (start.x != end.x && start.y != end.y);
        int length = max(abs(start.x - end.x), abs(start.y - end.y));
        for (int j = 0; j <= length; j++) {
            Point2 p = {
                .x = start.x + j * sign(end.x - start.x),
                .y = start.y + j * sign(end.y - start.y)
            };
            if (!diag) {
                orth_overlap_count += (orth_intersections[p.y][p.x] == 1);
                ++orth_intersections[p.y][p.x];
            }
            overlap_count += intersections[p.y][p.x] == 1;
            ++intersections[p.y][p.x];
        }
    }
    free(orth_intersections);
    free(intersections);
    da_free(segments);

    printf("Part 1: %d\n", orth_overlap_count);
    printf("Part 2: %d\n", overlap_count);
}