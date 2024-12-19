#include <stdio.h>
#include <stdbool.h>

#define _LINE_MAX 256

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"

typedef enum {
    UP, RIGHT, DOWN, LEFT,
} Direction;

typedef struct {
    Point2 pos;
    Direction direction;
} Reindeer;

typedef struct {
    Reindeer reindeer;
    int points;
    int distance;
    Point2* path;
} State;

bool* parse_input(FILE* input, int* stride, Point2* start_pos, Point2* end_pos) {
    bool* map = da_create(sizeof(bool));
    char buffer[_LINE_MAX] = { 0 };
    *stride = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        if (!*stride)
            *stride = strcspn(buffer, "\r\n");
        for (int i = 0; i < *stride; i++) {
            if (buffer[i] == 'S') {
                *start_pos = (Point2){ .x = i, .y = da_length(map) / (*stride) };
            } else if (buffer[i] == 'E') {
                *end_pos = (Point2){ .x = i, .y = da_length(map) / (*stride) };
            }
            da_append(map, (bool) { buffer[i] != '#' });
        }
    }
    return map;
}

int state_cmp(const void* a, const void* b) {
    State* sa = (State*)a;
    State* sb = (State*)b;
    if (sb->points == sa->points) {
        return sb->distance == sa->distance;
    }
    return sb->points - sa->points;
}

static inline int mod(int x, int m) {
    return (x % m + m) % m;
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");

    int stride;
    Point2 start_pos, end_pos;
    bool* map = parse_input(input, &stride, &start_pos, &end_pos);
    fclose(input);

    Hmap* visited = hm_create(sizeof(Reindeer), sizeof(int));
    Pqueue pq = pq_create(sizeof(State), state_cmp);
    State current = { .reindeer = {.pos = start_pos, .direction = RIGHT }, .points = 0 , .path = da_create(sizeof(Point2)), .distance = p2dist(start_pos, end_pos) };
    da_append(current.path, start_pos);
    pq_push(pq, current);
    int* best_points_here;

    int end_points = -1;
    Point2** best_paths = da_create(sizeof(Point2*));

    while (pq_length(pq)) {
        pq_pop(pq, &current);

        if (current.distance == 0) {
            end_points = current.points;
            da_append(best_paths, current.path);
            continue;
        } else if (end_points != -1) {
            da_free(current.path);
            break;
        }
        if ((best_points_here = hm_search(*visited, &current.reindeer)) != NULL) {
            if (*best_points_here < current.points) {
                da_free(current.path);
                continue;
            }
        } else {
            hm_insert(visited, &current.reindeer, &current.points);
        }


        Point2 next_pos = p2add(current.reindeer.pos, p2adjacents[current.reindeer.direction]);
        if (map[next_pos.y * stride + next_pos.x]) {
            State next_state = { .reindeer = {.pos = next_pos, .direction = current.reindeer.direction }, .points = current.points + 1 };
            next_state.path = da_copy(current.path);
            da_append(next_state.path, next_pos);
            next_state.distance = p2dist(next_pos, end_pos);
            pq_push(pq, next_state);
        }

        State next_state = current;
        next_state.points += 1000;
        next_state.reindeer.direction = mod(current.reindeer.direction + 1, 4);
        next_state.path = da_copy(current.path);
        pq_push(pq, next_state);
        next_state.reindeer.direction = mod(current.reindeer.direction - 1, 4);
        next_state.path = da_copy(current.path);
        pq_push(pq, next_state);
        da_free(current.path);
    }

    while (pq_length(pq)) {
        State current;
        pq_pop(pq, &current);
        da_free(current.path);
    }

    hm_free(visited);
    pq_free(pq);
    da_free(map);

    Hset* tiles = hs_create(sizeof(Point2));
    for (size_t i = 0; i < da_length(best_paths); ++i) {
        for (size_t j = 0; j < da_length(best_paths[i]); ++j) {
            hs_insert(tiles, &best_paths[i][j]);
        }
        da_free(best_paths[i]);
    }
    int tiles_cnt = hs_length(*tiles);
    hs_free(tiles);
    da_free(best_paths);

    printf("Part 1: %d\n", end_points);
    printf("Part 2: %d\n", tiles_cnt);
}