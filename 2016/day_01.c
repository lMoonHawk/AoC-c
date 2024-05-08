#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

const Point2 moves[] = {{0,-1}, {1,0}, {0,1}, {-1,0}}; 

int main() {
    FILE* input = fopen("data/day_01.txt", "r");

    Point2 pos = {0};
    Point2 *visited = da_create(sizeof(Seg2));
 
    int facing = 0;
    int steps = 0;

    Point2 *location = NULL;

    char c = fgetc(input);
    while(1) {
        if (c == 'R') {
            if(++facing == 4) facing = 0;
        } 
        else if (c == 'L') {
            if(--facing == -1) facing = 3;
        }
        else if (isdigit(c)) {
            steps = steps * 10 + c - '0';
        }
        else {
            for (int i = 0; i < steps; ++i) {
                pos = p2add(pos, moves[facing]);

                if (location == NULL) {
                    for (size_t j = 0; j < da_length(visited); ++j) {
                        if(pos.x == visited[j].x && pos.y == visited[j].y) {
                            location = malloc(sizeof(Point2));
                            *location = pos;
                        }
                    }
                    da_append(visited, pos);
                }
            }
            steps = 0;
            if (c == EOF) 
                break; 
        }
        c = fgetc(input);
    }

    printf("Part 1: %d\n", p2dist(pos, (Point2){0}));
    printf("Part 2: %d\n", p2dist(*location, (Point2){0}));

    da_free(visited);
    free(location);
    fclose(input);
}