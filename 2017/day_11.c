#include <stdio.h>
#include <string.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

// Hexagonal tiles are mapped to cartesian coordinates.
// A side normal vector are the sum of the adjacent sides normals i.e. ne = n + se / (1,1) = (0,1) + (1,0)
Point2 hex_to_cart(const char* hex_direction) {
    if (strcmp(hex_direction, "n") == 0) {
        return (Point2) { 0, 1 };
    } else if (strcmp(hex_direction, "ne") == 0) {
        return (Point2) { 1, 1 };
    } else if (strcmp(hex_direction, "se") == 0) {
        return (Point2) { 1, 0 };
    } else if (strcmp(hex_direction, "s") == 0) {
        return (Point2) { 0, -1 };
    } else if (strcmp(hex_direction, "sw") == 0) {
        return (Point2) { -1, -1 };
    } else if (strcmp(hex_direction, "nw") == 0) {
        return (Point2) { -1, 0 };
    }
    return (Point2) { 0 };
}

// The L1 distance has to be modified as it is possible to reach (+1,+1) or (-1,-1) in a single step.
int hex_dist_zero(Point2 p1) {
    if (p1.x * p1.y > 0)
        return abs(p1.x) > abs(p1.y) ? abs(p1.x) : abs(p1.y);
    return abs(p1.x - p1.y);
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");

    Point2 pos = { 0 };
    int max_dist = 0;

    char c;
    char hex_direction[3] = { 0 };
    char* s = hex_direction;
    while ((c = fgetc(input)) != EOF) {
        if (c == ',' || c == '\r' || c == '\n') {
            pos = p2add(pos, hex_to_cart(hex_direction));
            int dist = hex_dist_zero(pos);
            if (dist > max_dist)
                max_dist = dist;

            memset(hex_direction, 0, 3);
            s = hex_direction;

        } else {
            *(s++) = c;
        }
    }
    fclose(input);

    printf("Part 1: %d\n", hex_dist_zero(pos));
    printf("Part 2: %d\n", max_dist);
}