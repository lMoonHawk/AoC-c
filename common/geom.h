#ifndef GEOM_H
#define GEOM_H

#include <stdbool.h>

typedef struct { int x, y; } Point2;
typedef struct { Point2 min, max; } P2Bounds;
typedef struct { Point2 start, end; } Seg2;

Point2 p2add(Point2 p1, Point2 p2);
Point2 p2scale(Point2 v, int s);
int abs(int i);
int p2dist(Point2 p1, Point2 p2);
bool p2in_bounds(Point2 v, P2Bounds b);

#endif // GEOM_H

#ifdef GEOM_IMPLEMENTATION

Point2 p2add(Point2 p1, Point2 p2) {
    return (Point2) { .x = p1.x + p2.x, .y = p1.y + p2.y };
}

Point2 p2mul(Point2 p1, Point2 p2) {
    return (Point2) { .x = p1.x * p2.x, .y = p1.y * p2.y };
}

Point2 p2scale(Point2 v, int s) {
    return (Point2) { .x = v.x * s, .y = v.y * s };
}

bool p2in_bounds(Point2 v, P2Bounds b) {
    return (v.x >= b.min.x && v.y >= b.min.y && v.x <= b.max.x && v.y <= b.max.y);
}




int abs(int i) {
    return (i > 0) ? i : -i;
}

int p2dist(Point2 p1, Point2 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

bool get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y, float* i_x, float* i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

#endif // GEOM_IMPLEMENTATION