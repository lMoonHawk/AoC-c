#ifndef GEOM_H
#define GEOM_H

#include <stdbool.h>
#include <limits.h>

typedef enum { P2_UP, P2_RIGHT, P2_DOWN, P2_LEFT, P2_ADJCOUNT } P2Direction;

typedef union {
    struct { int x, y; };
    int components[2];
} Point2;
typedef union {
    struct { int x, y, z; };
    int components[3];
} Point3;
typedef struct { Point2 min, max; } P2Bounds;
typedef struct { Point2 start, end; } Seg2;

Point2 p2add(Point2 p1, Point2 p2);
Point3 p3add(Point3 p1, Point3 p2);
Point2 p2mul(Point2 p1, Point2 p2);
Point3 p3mul(Point3 p1, Point3 p2);
Point2 p2scale(Point2 v, int s);
Point3 p3scale(Point3 v, int s);
int sign(int i);
int abs(int i);
int p2dist(Point2 p1, Point2 p2);
int p3dist(Point3 p1, Point3 p2);
P2Bounds p2bounds_max();
bool p2in_bounds(Point2 v, P2Bounds b);
void p2bounds_update(P2Bounds* b, Point2 p);

#endif // GEOM_H

#ifdef GEOM_IMPLEMENTATION

Point2 p2adjacents[4] = {
    {.x = 0, .y = -1},
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0}
};

Point2 p2add(Point2 p1, Point2 p2) {
    return (Point2) { .x = p1.x + p2.x, .y = p1.y + p2.y };
}

Point3 p3add(Point3 p1, Point3 p2) {
    return (Point3) { .x = p1.x + p2.x, .y = p1.y + p2.y, .z = p1.z + p2.z };
}

Point2 p2mul(Point2 p1, Point2 p2) {
    return (Point2) { .x = p1.x * p2.x, .y = p1.y * p2.y };
}

Point3 p3mul(Point3 p1, Point3 p2) {
    return (Point3) { .x = p1.x * p2.x, .y = p1.y * p2.y, .z = p1.z * p2.z };
}

Point2 p2scale(Point2 v, int s) {
    return (Point2) { .x = v.x * s, .y = v.y * s };
}

Point3 p3scale(Point3 v, int s) {
    return (Point3) { .x = v.x * s, .y = v.y * s, .z = v.z * s };
}

P2Bounds p2bounds_max() {
    return (P2Bounds) { .min = { .x = INT_MAX, .y = INT_MAX }, .max = { .x = INT_MIN, .y = INT_MIN } };
}

bool p2in_bounds(Point2 v, P2Bounds b) {
    return (v.x >= b.min.x && v.y >= b.min.y && v.x <= b.max.x && v.y <= b.max.y);
}

void p2bounds_update(P2Bounds* b, Point2 p) {
    if (b->min.x > p.x) b->min.x = p.x;
    if (b->min.y > p.y) b->min.y = p.y;
    if (b->max.x < p.x) b->max.x = p.x;
    if (b->max.y < p.y) b->max.y = p.y;
}
int sign(int i) {
    return (i > 0) ? 1 : (i < 0) ? -1 : 0;
}
int abs(int i) {
    return (i > 0) ? i : -i;
}

int p2dist(Point2 p1, Point2 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int p3dist(Point3 p1, Point3 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y) + abs(p1.z - p2.z);
}


#endif // GEOM_IMPLEMENTATION