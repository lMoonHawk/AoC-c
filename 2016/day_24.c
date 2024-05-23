#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

Point2 moves[] = { {1, 0}, {0, 1}, {-1, 0}, { 0, -1} };

typedef struct {
    int bitset;
    Point2 pos;
} State;

void collect_waypoint(State* s, State* wps, int goal) {
    for (size_t i = 0; i < da_length(wps); ++i) {
        if (wps[i].pos.x == s->pos.x && wps[i].pos.y == s->pos.y) {
            // Only allow waypoint '0' if its the last
            if (wps[i].bitset != 1 || s->bitset == goal - 1)
                s->bitset |= wps[i].bitset;
            break;
        }
    }
}

bool visit(State s, bool* v, size_t width, size_t height) {
    size_t state_hash = s.pos.x + width * (s.pos.y + height * s.bitset);
    if (v[state_hash])
        return false;
    v[state_hash] = true;
    return true;
}

void traverse(bool* layout, State* waypoints, size_t width, size_t height, int* min_steps, int* min_steps_round) {
    P2Bounds bounds = { {0, 0}, {width - 1, height - 1} };
    Queue q = q_create(sizeof(State));

    int goal = (1 << da_length(waypoints)) - 1;
    bool* visited = calloc(width * height * goal, sizeof(bool));

    State init = { 0 };
    for (size_t i = 0; i < da_length(waypoints); ++i) {
        if (waypoints[i].bitset == 1)
            init = waypoints[i];
    }
    init.bitset = 0;
    q_push(q, init);

    int steps = 0;
    int depth_nodes = 1;
    while (q_length(q)) {
        if (depth_nodes == 0) {
            ++steps;
            depth_nodes = q_length(q);
        }

        State current;
        q_pop(q, &current);

        if (*min_steps == 0 && current.bitset == goal - 1) {
            *min_steps = steps;
        } else if (current.bitset == goal) {
            *min_steps_round = steps;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            Point2 new_pos = p2add(current.pos, moves[i]);
            if (!p2in_bounds(new_pos, bounds))
                continue;
            if (!layout[new_pos.x + new_pos.y * width])
                continue;

            State new_state = { current.bitset, new_pos };
            if (!visit(new_state, visited, width, height))
                continue;

            collect_waypoint(&new_state, waypoints, goal);

            q_push(q, new_state);

        }
        --depth_nodes;
    }
    free(visited);
    q_free(q);
}

void parse_input(FILE* input, bool** layout, State** waypoints, size_t* width, size_t* height) {
    char c;
    *width = INT_MAX;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\r' || c == '\n') {
            if (*width == INT_MAX)
                *width = da_length(*layout);
            continue;
        }
        if (isdigit(c)) {
            int x = da_length(*layout) % *width;
            int y = da_length(*layout) / *width;
            int bitset = 1 << (c - '0');
            da_append(*waypoints, ((State) {bitset, { x, y }}));
        }
        da_append(*layout, ((bool) {c != '#'}));
    }
    *height = da_length(*layout) / *width;
}

int main() {
    FILE* input = fopen("data/day_24.txt", "r");

    bool* layout = da_create(sizeof(bool));
    State* waypoints = da_create(sizeof(State));
    size_t width, height;
    parse_input(input, &layout, &waypoints, &width, &height);
    fclose(input);

    int min_steps = 0, min_steps_round = 0;
    traverse(layout, waypoints, width, height, &min_steps, &min_steps_round);

    printf("Part 1: %d\n", min_steps);
    printf("Part 2: %d\n", min_steps_round);

    da_free(layout);
    da_free(waypoints);
}