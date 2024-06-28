#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

typedef enum {
    NONE = 1,
    TORCH = 1 << 1,
    GEAR = 1 << 2,
} Tool;

typedef struct {
    Point2 position;
    Tool tool;
} State;

typedef struct {
    State state;
    int minutes;
    int cost;
} Node;

uint16_t get_erosion(Point2 p, Point2 target, int depth, uint16_t*** erosions) {
    assert(p.x < 10000 && p.y < 10000);
    if (p.y < (int)da_length((*erosions)) && p.x < (int)da_length((*erosions)[p.y]))
        return (*erosions)[p.y][p.x];

    if (p.y == (int)da_length(*erosions)) {
        uint16_t* row = da_create(sizeof(uint16_t));
        da_append(*erosions, row);
    }
    int geo_level = 0;
    if (p.y == target.y && p.x == target.x)
        geo_level = 0;
    else if (p.y == 0)
        geo_level = p.x * 16807;
    else if (p.x == 0)
        geo_level = p.y * 48271;
    else {
        uint16_t top = get_erosion(p2add(p, (Point2) { 0, -1 }), target, depth, erosions);
        uint16_t left = get_erosion(p2add(p, (Point2) { -1, 0 }), target, depth, erosions);
        geo_level = top * left;
    }
    uint16_t erosion = (geo_level + depth) % 20183;
    if (p.x == (int)da_length((*erosions)[p.y]))
        da_append((*erosions)[p.y], erosion);
    return erosion;
}

uint8_t get_type(Point2 p, Point2 target, int depth, uint16_t*** erosions) {
    return get_erosion(p, target, depth, erosions) % 3;
}

int nodecmp(const void* a, const void* b) {
    Node* n1 = (Node*)a;
    Node* n2 = (Node*)b;
    if (n1->cost == n2->cost)
        return n2->minutes - n1->minutes;
    return n2->cost - n1->cost;
}

void parse_input(FILE* input, int* depth, Point2* target) {
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, LINE_MAX, input);
    strtok(buffer, " ");
    *depth = atoi(strtok(NULL, " "));
    fgets(buffer, LINE_MAX, input);
    strtok(buffer, " ");
    target->x = atoi(strtok(NULL, ","));
    target->y = atoi(strtok(NULL, " "));
}

int main() {
    FILE* input = fopen("data/day_22.txt", "r");
    Point2 target;
    int depth;
    parse_input(input, &depth, &target);
    fclose(input);

    uint16_t** erosions = da_create(sizeof(uint16_t*));
    int risk_level = 0;
    for (int y = 0; y <= target.y; ++y)
        for (int x = 0; x <= target.x; ++x)
            risk_level += get_type((Point2) { x, y }, target, depth, & erosions);


    uint8_t regions_tools[3] = { TORCH | GEAR, NONE | GEAR, NONE | TORCH };
    Point2 moves[4] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };

    Hset* visited = hs_create(sizeof(State));

    Pqueue pq = pq_create(sizeof(Node), nodecmp);
    Node current = { { { 0 , 0 }, TORCH }, 0, 0 };
    pq_push(pq, current);

    while (pq_length(pq)) {
        pq_pop(pq, &current);
        if (hs_contains(*visited, &current.state))
            continue;
        hs_insert(visited, &current.state);
        if (current.state.position.x == target.x && current.state.position.y == target.y) {
            if (current.state.tool == TORCH)
                break;
            current.state.tool = TORCH;
            current.minutes += 7;
            pq_push(pq, current);
            continue;
        }
        for (int i = 0; i < 4; ++i) {
            Point2 next_position = p2add(current.state.position, moves[i]);
            if (next_position.x < 0 || next_position.y < 0)
                continue;
            uint8_t current_type = get_type(current.state.position, target, depth, &erosions);
            uint8_t next_type = get_type(next_position, target, depth, &erosions);
            uint8_t allowed = regions_tools[current_type] & regions_tools[next_type];
            for (Tool tool = NONE; tool <= GEAR; tool <<= 1) {
                if (!(allowed & tool))
                    continue;
                int new_minutes = current.minutes + 1;
                if (tool != current.state.tool)
                    new_minutes += 7;
                int new_cost = new_minutes + abs(target.x - next_position.x) + abs(target.y - next_position.y);
                Node next = { { next_position, tool }, new_minutes, new_cost };
                pq_push(pq, next);
            }
        }
    }
    for (size_t i = 0; i < da_length(erosions); ++i)
        da_free(erosions[i]);
    da_free(erosions);
    hs_free(visited);
    pq_free(pq);

    printf("Part 1: %d\n", risk_level);
    printf("Part 2: %d\n", current.minutes);
}