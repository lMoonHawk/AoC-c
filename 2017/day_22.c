#include <stdio.h>
#include <stdlib.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

typedef enum {
    CLEAN,
    WEAKENED,
    INFECTED,
    FLAGGED,
} State;

int mod(int a, int b) {
    return ((a % b) + b) % b;
}

int count_infections_1(Hmap* grid, Point2 pos) {
    Point2 directions[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

    int facing = 0;
    int infect_cnt = 0;
    for (int i = 0; i < 10000; ++i) {
        State* pos_state = (State*)hm_get(*grid, &pos);
        if (pos_state == NULL || *pos_state == CLEAN) {
            facing = mod(facing - 1, 4);
            hm_set(grid, &pos, &(State){INFECTED});
            ++infect_cnt;
        } else {
            facing = mod(facing + 1, 4);
            hm_set(grid, &pos, &(State){CLEAN});
        }
        pos = p2add(pos, directions[facing]);
    }
    return infect_cnt;
}

int count_infections_2(Hmap* grid, Point2 pos) {
    Point2 directions[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

    int facing = 0;
    int infect_cnt = 0;
    for (int i = 0; i < 10000000; ++i) {
        State* cached_state = (State*)hm_get(*grid, &pos);
        State pos_state = cached_state == NULL ? CLEAN : *cached_state;
        switch (pos_state) {
            case CLEAN:
                facing = mod(facing - 1, 4);
                hm_set(grid, &pos, &(State){WEAKENED});
                break;
            case WEAKENED:
                hm_set(grid, &pos, &(State){INFECTED});
                ++infect_cnt;
                break;
            case INFECTED:
                facing = mod(facing + 1, 4);
                hm_set(grid, &pos, &(State){FLAGGED});
                break;
            case FLAGGED:
                facing = mod(facing + 2, 4);
                hm_set(grid, &pos, &(State){CLEAN});
                break;
        }
        pos = p2add(pos, directions[facing]);
    }
    return infect_cnt;
}

Hmap* parse_input(FILE* input, Point2* start) {
    Hmap* hm = hm_create(sizeof(Point2), sizeof(State));

    char buffer[LINE_MAX] = { 0 };
    int x = 0, y = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        x = 0;
        for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r'; ++i) {
            Point2 point = { x, y };
            State state = buffer[i] == '#' ? INFECTED : CLEAN;
            hm_set(hm, &point, &state);
            ++x;
        }
        ++y;
    }
    *start = (Point2){ x / 2, y / 2 };
    return hm;
}

int main() {
    FILE* input = fopen("data/day_22.txt", "r");
    Point2 pos;
    Hmap* grid_1 = parse_input(input, &pos);
    fclose(input);
    Hmap* grid_2 = hm_copy(*grid_1);

    printf("Part 1: %d\n", count_infections_1(grid_1, pos));
    printf("Part 2: %d\n", count_infections_2(grid_2, pos));
    
    hm_free(grid_1);
    hm_free(grid_2);
}