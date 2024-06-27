#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} Direction;

Direction get_direction(char c) {
    switch (c) {
        case 'N': return NORTH;
        case 'E': return EAST;
        case 'S': return SOUTH;
        case 'W': return WEST;
    }
    return 0;
}

Point2 directions[4] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

void add_node(Hmap* graph, Point2 position, Point2 new_position, Direction direction) {
    bool* tmp = calloc(4, sizeof(bool));

    bool* value = hm_search(*graph, &position);
    if (value == NULL) {
        hm_insert(graph, &position, tmp);
        value = hm_search(*graph, &position);
    }
    value[direction] = true;

    Direction opposite_direction = (direction + 2) % 4;
    bool* opposite_value = hm_search(*graph, &new_position);
    if (opposite_value == NULL) {
        hm_insert(graph, &new_position, tmp);
        opposite_value = hm_search(*graph, &new_position);
    }
    opposite_value[opposite_direction] = true;

    free(tmp);
}

Hmap* parse_input(FILE* input) {
    Hmap* graph = hm_create(sizeof(Point2), 4 * sizeof(bool));
    Point2* stack = da_create(sizeof(*stack));
    Point2 position = { 0, 0 };
    Point2 start = position;
    char c = fgetc(input);
    while (c != '\n' && c != '\r' && c != EOF) {
        switch (c) {
            case 'N': case 'S': case 'E': case 'W':
                Point2 new_position = p2add(position, directions[get_direction(c)]);
                add_node(graph, position, new_position, get_direction(c));
                position = new_position;
                break;
            case '(':
                da_append(stack, start);
                start = position;
                break;
            case ')':
                da_pop(stack, &start);
                break;
            case '|':
                position = start;
                break;
        }
        c = fgetc(input);
    }
    da_free(stack);
    return graph;
}

int main() {
    FILE* input = fopen("data/day_20.txt", "r");
    Hmap* graph = parse_input(input);
    fclose(input);

    Queue queue = q_create(sizeof(Point2));
    q_push(queue, ((Point2) { 0, 0 }));

    Hset* visited = hs_create(sizeof(Point2));
    int steps = 0;
    int depth_nodes = 1;
    int above_thousand = 0;
    while (q_length(queue)) {
        if (depth_nodes == 0) {
            ++steps;
            depth_nodes = q_length(queue);
        }
        if (steps >= 1000)
            ++above_thousand;

        Point2 position;
        q_pop(queue, &position);
        hs_insert(visited, &position);

        bool* valid_directions = hm_search(*graph, &position);
        for (int i = 0; i < 4; ++i) {
            Point2 new_position = p2add(position, directions[i]);
            if (valid_directions[i] && !hs_contains(*visited, &new_position))
                q_push(queue, new_position);
        }
        --depth_nodes;
    }
    hs_free(visited);
    hm_free(graph);
    q_free(queue);

    printf("Part 1: %d\n", steps);
    printf("Part 2: %d\n", above_thousand);
    
}