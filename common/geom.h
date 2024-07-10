#ifndef GEOM_H
#define GEOM_H

#include <stdbool.h>

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
int abs(int i);
int p2dist(Point2 p1, Point2 p2);
int p3dist(Point3 p1, Point3 p2);
bool p2in_bounds(Point2 v, P2Bounds b);

#endif // GEOM_H

#ifdef GEOM_IMPLEMENTATION

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

bool p2in_bounds(Point2 v, P2Bounds b) {
    return (v.x >= b.min.x && v.y >= b.min.y && v.x <= b.max.x && v.y <= b.max.y);
}

int abs(int i) {
    return (i > 0) ? i : -i;
}

int p2dist(Point2 p1, Point2 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int p3dist(Point3 p1, Point3 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y) + +abs(p1.z - p2.z);
}


#endif // GEOM_IMPLEMENTATION