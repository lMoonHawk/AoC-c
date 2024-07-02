#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

bool S2intersect(Seg2 s1, Seg2 s2, Point2* intersection) {
    if ((s1.start.x == s1.end.x) == (s2.start.x == s2.end.x))
        return false;

    Seg2 horizontal = s1;
    Seg2 vertical = s2;
    if (s1.start.y == s1.end.y) {
        horizontal = s2;
        vertical = s1;
    }
    Point2 i = { horizontal.start.x, vertical.start.y };
    if (((i.x >= vertical.start.x && i.x <= vertical.end.x) || (i.x <= vertical.start.x && i.x >= vertical.end.x)) && 
    ((i.y >= horizontal.start.y && i.y <= horizontal.end.y) || (i.y <= horizontal.start.y && i.y >= horizontal.end.y))) {
        *intersection = i;
        return true;
    }
    return false;
}

Point2 parse_direction(char c) {
    switch (c) {
        case 'R':
            return (Point2) { 1, 0 };
        case 'L':
            return (Point2) { -1, 0 };
        case 'U':
            return (Point2) { 0, -1 };
        case 'D':
            return (Point2) { 0, 1 };
    }
    assert(0 && "Unknown direction");
}

Seg2* parse_wire(FILE* input) {
    Seg2* wire = da_create(sizeof(Seg2));
    Point2 start = { 0 };
    Point2 end = { 0 };
    char c = fgetc(input);
    while (c != 'R' && c != 'L' && c != 'U' && c != 'D')
        c = fgetc(input);
    while (c != '\r' && c != '\n' && c != EOF) {
        Point2 direction = parse_direction(c);
        int length = 0;
        c = fgetc(input);
        while (isdigit(c)) {
            length = length * 10 + (c - '0');
            c = fgetc(input);
        }
        end = p2add(start, p2scale(direction, length));
        Seg2 segment = { .start = start, .end = end };
        da_append(wire, segment);
        start = end;
        c = fgetc(input);
    }
    return wire;
}

void parse_input(FILE* input, Seg2** wire1, Seg2** wire2) {
    *wire1 = parse_wire(input);
    *wire2 = parse_wire(input);
}

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    Seg2* wire1 = NULL;
    Seg2* wire2 = NULL;
    parse_input(input, &wire1, &wire2);
    fclose(input);

    int best_distance = INT_MAX;
    int best_steps = INT_MAX;
    int wire1_length = 0;
    int wire2_length = 0;
    for (size_t i = 0; i < da_length(wire1); ++i) {
        for (size_t j = 0; j < da_length(wire2); ++j) {
            Point2 intersection = { 0 };
            if (S2intersect(wire1[i], wire2[j], &intersection)) {
                int dist = p2dist(intersection, (Point2) { 0 });
                if (dist < best_distance)
                    best_distance = dist;
                int steps = wire1_length + wire2_length;
                steps += p2dist(wire1[i].start, intersection);
                steps += p2dist(wire2[j].start, intersection);
                if (steps < best_steps)
                    best_steps = steps;
            }
            wire2_length += p2dist(wire2[j].start, wire2[j].end);
        }
        wire1_length += p2dist(wire1[i].start, wire1[i].end);
        wire2_length = 0;
    }
    da_free(wire1);
    da_free(wire2);

    printf("Part 1: %d\n", best_distance);
    printf("Part 2: %d\n", best_steps);
}