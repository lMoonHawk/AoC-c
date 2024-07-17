#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

typedef struct {
    Point2 inner, outer;
    char label[2];
} Portal;

char* parse_input(FILE* input, size_t* maze_stride, P2Bounds* maze_bounds) {
    char* maze = da_create(1);
    *maze_bounds = (P2Bounds){
        {.x = INT_MAX, .y = INT_MAX},
        {.x = INT_MIN, .y = INT_MIN},
    };

    *maze_stride = 0;
    char c = fgetc(input);
    int x = 0, y = 0;
    while (c != EOF) {
        if (c == '\n') {
            ++y;
            x = 0;
            if (*maze_stride == 0)
                *maze_stride = da_length(maze);
        } else if (c != '\r') {
            da_append(maze, c);
            if (c == '#' || c == '.')
                p2bounds_update(maze_bounds, (Point2) { .x = x, .y = y });
            ++x;
        }
        c = fgetc(input);
    }
    return maze;
}

void add_portal(Portal** portals, char label[2], Point2 pos, bool inner) {
    for (size_t i = 0; i < da_length(*portals); ++i) {
        Portal* p = &(*portals)[i];
        if (label[0] == p->label[0] && label[1] == p->label[1]) {
            if (inner)
                p->inner = pos;
            else
                p->outer = pos;
            return;
        }
    }
    Portal portal = { 0 };
    portal.label[0] = label[0];
    portal.label[1] = label[1];
    if (inner)
        portal.inner = pos;
    else
        portal.outer = pos;
    da_append(*portals, portal);
}

Portal* get_portals_from_map(char* maze, size_t maze_stride, P2Bounds maze_bounds) {
    Portal* portals = da_create(sizeof(Portal));
    for (size_t i = 0; i < da_length(maze); ++i) {
        int y = i / maze_stride;
        int x = i % maze_stride;
        Point2 xy = { .x = x, .y = y };
        char cell = maze[y * maze_stride + x];

        if (cell >= 'A' && cell <= 'Z') {
            char label[2] = { 0 };
            if ((y - 1 >= 0) && (maze[(y - 1) * maze_stride + x] >= 'A') && (maze[(y - 1) * maze_stride + x] <= 'Z')) {
                label[0] = maze[(y - 1) * maze_stride + x];
                label[1] = cell;
                if ((y - 2 >= 0) && (maze[(y - 2) * maze_stride + x] == '.')) {
                    Point2 pos = { .x = x, .y = y - 2 };
                    add_portal(&portals, label, pos, p2in_bounds(xy, maze_bounds));
                }
                if ((y + 1 >= 0) && (maze[(y + 1) * maze_stride + x] == '.')) {
                    Point2 pos = { .x = x, .y = y + 1 };
                    add_portal(&portals, label, pos, p2in_bounds(xy, maze_bounds));
                }
            }
            if ((x - 1 >= 0) && (maze[y * maze_stride + x - 1] >= 'A') && (maze[y * maze_stride + x - 1] <= 'Z')) {
                label[0] = maze[y * maze_stride + x - 1];
                label[1] = cell;
                if ((x - 2 >= 0) && (maze[y * maze_stride + x - 2] == '.')) {
                    Point2 pos = { .x = x - 2, .y = y };
                    add_portal(&portals, label, pos, p2in_bounds(xy, maze_bounds));
                }
                if ((x + 1 >= 0) && (maze[y * maze_stride + x + 1] == '.')) {
                    Point2 pos = { .x = x + 1, .y = y };
                    add_portal(&portals, label, pos, p2in_bounds(xy, maze_bounds));
                }
            }
        }
    }
    return portals;
}

bool traverse_portal(Point2 pos, Portal* portals, Point2* position, int* next_level) {
    for (size_t i = 0; i < da_length(portals); ++i) {
        if (pos.x == portals[i].outer.x && pos.y == portals[i].outer.y && *next_level > 0) {
            *position = portals[i].inner;
            --(*next_level);
            return true;
        }
        if (pos.x == portals[i].inner.x && pos.y == portals[i].inner.y) {
            *position = portals[i].outer;
            ++(*next_level);
            return true;
        }
    }
    return false;
}


int main() {
    FILE* input = fopen("data/day_20.txt", "r");

    size_t maze_stride = 0;
    P2Bounds maze_bounds;
    char* maze = parse_input(input, &maze_stride, &maze_bounds);
    fclose(input);

    Portal* portals = get_portals_from_map(maze, maze_stride, maze_bounds);


    Point2 start, end;
    for (size_t i = 0; i < da_length(portals); ++i) {
        if (strncmp(portals[i].label, "AA", 2) == 0) {
            start = portals[i].outer;
            da_remove(portals, i--);
        } else if (strncmp(portals[i].label, "ZZ", 2) == 0) {
            end = portals[i].outer;
            da_remove(portals, i--);
        }
    }
    typedef struct {
        Point2 position;
        int level;
    } State;

    Queue q = q_create(sizeof(State));
    Hset* visited = hs_create(sizeof(State));
    State init = { .position = start, .level = 0 };
    q_push(q, init);
    int steps_p1 = 0;
    int steps_p2 = 0;
    int depth_nodes = 1;

    while (q_length(q)) {
        if (depth_nodes == 0) {
            depth_nodes = q_length(q);
            ++steps_p2;
        }
        State current;
        q_pop(q, &current);

        if (current.position.x == end.x && current.position.y == end.y) {
            if (current.level == 0)
                break;
            if (steps_p1 == 0)
                steps_p1 = steps_p2;
        }
        hs_insert(visited, &current);

        for (int i = 0; i < 4; ++i) {
            State next = current;
            next.position = p2add(current.position, p2adjacents[i]);
            char cell = maze[next.position.y * maze_stride + next.position.x];
            if (cell != '.') {
                if (cell == '#') {
                    continue;
                } else {
                    if (!traverse_portal(current.position, portals, &next.position, &next.level))
                        continue;
                }
            }
            if (hs_contains(*visited, &next))
                continue;
            q_push(q, next);
        }
        --depth_nodes;
    }
    hs_free(visited);
    q_free(q);
    da_free(portals);
    da_free(maze);

    printf("Part 1: %d\n", steps_p1);
    printf("Part 2: %d\n", steps_p2);
}