#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

typedef enum {
    WALL,
    EMPTY,
    OXYGEN,
} Type;

Hmap* explore_area(int64_t* program, Point2* oxygen, int* steps_to_oxygen) {
    Hmap* area = hm_create(sizeof(Point2), sizeof(Type));
    Point2 moves[5] = { {.x = 0, .y = 0}, {.x = 0, .y = -1}, {.x = 0, .y = 1}, {.x = -1, .y = 0}, {.x = 1, .y = 0} };
    Point2 position = { .x = 0, .y = 0 };
    IC_state* state = ic_create(program);
    hm_insert(area, &position, (void*)(&(Type) { EMPTY }));

    typedef struct {
        Point2 position;
        IC_state* state;
    } Node;
    Queue q = q_create(sizeof(Node));
    q_push(q, ((Node){ position, state}));

    int depth_nodes = 1;
    int steps = 0;
    while (q_length(q)) {
        if (depth_nodes == 0) {
            ++steps;
            depth_nodes = q_length(q);
        }
        Node current;
        q_pop(q, &current);
        for (int input = 1; input <= 4; ++input) {
            Point2 next_position = p2add(current.position, moves[input]);
            if (hm_contains(*area, &next_position))
                continue;
            IC_state* next_state = ic_copy(current.state);
            ic_input_push(next_state, input);
            ic_interpret(next_state);
            Type type = ic_output_pop(next_state);
            hm_insert(area, &next_position, &type);

            if (type == OXYGEN) {
                *oxygen = next_position;
                *steps_to_oxygen = steps + 1;
            }
            if (type != WALL) {
                q_push(q, ((Node){ next_position, next_state }));
            } else {
                ic_free(next_state);
            }
        }
        --depth_nodes;
        ic_free(current.state);
    }
    q_free(q);
    return area;
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    Point2 oxygen = { 0 };
    int steps_to_oxygen;
    Hmap* area = explore_area(program, &oxygen, &steps_to_oxygen);

    Point2 moves[4] = { {.x = 0, .y = -1}, {.x = 0, .y = 1}, {.x = -1, .y = 0}, {.x = 1, .y = 0} };
    int empty_cells = 0;
    for (size_t i = 0; i < area->capacity; ++i)
        if (area->htable[i].key != NULL && *(Type*)area->htable[i].value == EMPTY)
            ++empty_cells;

    int filled_cells = 1;
    int steps = 0;
    Point2** oxygen_cells = da_create(sizeof(Point2*));
    while (filled_cells < empty_cells) {
        for (size_t i = 0; i < area->capacity; ++i)
            if (area->htable[i].key != NULL && *(Type*)area->htable[i].value == OXYGEN)
                da_append(oxygen_cells, ((Point2*){area->htable[i].key}));

        for (size_t i = 0; i < da_length(oxygen_cells); ++i) {
            for (int j = 0; j < 4; ++j) {
                Point2 neighbor = p2add(*(oxygen_cells[i]), moves[j]);

                Type* neighbor_type = hm_search(*area, &neighbor);
                if (*neighbor_type == EMPTY) {
                    *neighbor_type = OXYGEN;
                    ++filled_cells;
                }
            }
        }
        ++steps;
        da_set_length(oxygen_cells, 0);
    }
    da_free(program);
    hm_free(area);
    da_free(oxygen_cells);

    printf("Part 1: %d\n", steps_to_oxygen);
    printf("Part 1: %d\n", steps);
}