#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

typedef struct {
    int expected_risk;
    Point2 position;
} State;

uint8_t* parse_input(FILE* input, int* stride) {
    uint8_t* map = da_create(sizeof(*map));
    char c;
    int x = 0;
    while ((c = fgetc(input)) != EOF) {
        if (c != '\n' && c != '\r') {
            da_append(map, (uint8_t) { c - '0' });
            ++x;
        } else if (c == '\n') {
            *stride = x;
            x = 0;
        }
    }
    return map;
}

int state_cmp(const void* a, const void* b) {
    State* sa = (State*)a;
    State* sb = (State*)b;
    if (sa->expected_risk == sb->expected_risk) {
        if (sa->position.x == sb->position.x)
            return sa->position.y - sb->position.y;
        return sa->position.x - sb->position.x;
    }
    return sb->expected_risk - sa->expected_risk;
}

int find_best_path_score(uint8_t* map, int size) {
    int* best_so_far = calloc(size * size, sizeof(*best_so_far));
    Pqueue pq = pq_create(sizeof(State), state_cmp);
    State current = { 0 };
    pq_push(pq, current);

    while (pq_length(pq)) {
        pq_pop(pq, &current);
        if (current.position.x == size - 1 && current.position.y == size - 1)
            break;

        int current_risk = best_so_far[current.position.y * size + current.position.x];

        for (int k = 0; k < 4; ++k) {
            Point2 neigh = p2add(current.position, p2adjacents[k]);
            if (neigh.x < 0 || neigh.y < 0 || neigh.x >= size || neigh.y >= size)
                continue;
            int total_risk = current_risk + map[neigh.y * size + neigh.x];
            int cached_risk = best_so_far[neigh.y * size + neigh.x];
            if (cached_risk > 0 && cached_risk <= total_risk)
                continue;
            best_so_far[neigh.y * size + neigh.x] = total_risk;
            int heuristic = 2 * (size - 1) - neigh.x - neigh.y;
            State next = { total_risk + heuristic, neigh };
            pq_push(pq, next);
        }
    }
    free(best_so_far);
    pq_free(pq);
    return current.expected_risk;
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    int size;
    uint8_t* map = parse_input(input, &size);
    fclose(input);

    int best_score = find_best_path_score(map, size);

    uint8_t* large_map = malloc(size * 5 * size * 5);
    for (int y = 0; y < 5 * size; ++y) {
        for (int x = 0; x < 5 * size; ++x) {
            div_t new_y = div(y, size);
            div_t new_x = div(x, size);
            large_map[y * 5 * size + x] = (map[new_y.rem * size + new_x.rem] + new_x.quot + new_y.quot - 1) % 9 + 1;
        }
    }
    da_free(map);
    int best_score_large = find_best_path_score(large_map, 5 * size);
    free(large_map);

    printf("Part 1: %d\n", best_score);
    printf("Part 2: %d\n", best_score_large);
}