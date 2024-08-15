#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"

#define _LINE_MAX 256

Hset* parse_input(FILE* input) {
    Hset* dots = hs_create(sizeof(Point2));
    Point2 dot = { 0 };
    char buffer[_LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%d,%d", &dot.x, &dot.y);
        hs_insert(dots, &dot);
    }
    return dots;
}

Point2 fold_dot(Point2 dot, char axis, int line) {
    if (axis == 'x') {
        if (dot.x <= line)
            return dot;
        return (Point2) { .x = line * 2 - dot.x, .y = dot.y };
    }
    if (dot.y <= line)
        return dot;
    return (Point2) { .x = dot.x, .y = line * 2 - dot.y };
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");
    Hset* dots = parse_input(input);

    char buffer[_LINE_MAX] = { 0 };
    char axis;
    int line;
    P2Bounds bounds = { 0 };
    int first_fold_count = -1;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        bounds = p2bounds_max(dots);
        sscanf(buffer, "fold along %c=%d", &axis, &line);

        Hset* folded_dots = hs_create(sizeof(Point2));
        Hset_iter dots_iter = hs_iter(dots);
        Point2 dot;
        while (hs_iter_next(&dots_iter, &dot)) {
            Point2 folded_dot = fold_dot(dot, axis, line);
            hs_insert(folded_dots, &folded_dot);
            p2bounds_update(&bounds, folded_dot);
        }
        hs_free(dots);
        dots = folded_dots;
        if (first_fold_count == -1)
            first_fold_count = hs_length(*dots);
    }
    fclose(input);

    int cols = bounds.max.x + 2;
    int rows = bounds.max.y + 1;
    char* code = malloc(rows * cols);
    memset(code, ' ', rows * cols);
    for (int i = 0; i < rows; ++i)
        code[i * cols + cols - 1] = '\n';
    code[(rows - 1) * cols + cols - 1] = '\0';
    Hset_iter dots_iter = hs_iter(dots);
    Point2 dot;
    while (hs_iter_next(&dots_iter, &dot))
        code[dot.y * cols + dot.x] = '#';
    hs_free(dots);

    printf("Part 1: %d\n", first_fold_count);
    printf("Part 2: \n%s\n", code);
    free(code);
}