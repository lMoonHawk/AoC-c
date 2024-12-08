#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

typedef enum {
    UP, RIGHT, DOWN, LEFT
} Direction;

typedef struct {
    Point2 pos;
    Direction facing;
} State;

char* parse_input(FILE* input, int* stride) {
    char* map = da_create(1);
    *stride = 0;
    int i = 0;
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n' && *stride == 0) {
            *stride = i;
        } else if (c != '\n' && c != '\r') {
            ++i;
            da_append(map, c);
        }
    }
    return map;

}

bool is_loop(int rows, int cols, char (*map)[cols], Point2 pos, Direction facing) {
    int loop_threshold = rows * cols * 2;
    int path_len = 0;
    while (1) {
        if (path_len > loop_threshold)
            return true;
        Point2 next_pos = p2add(p2adjacents[facing], pos);
        if (next_pos.x < 0 || next_pos.x >= cols || next_pos.y < 0 || next_pos.y >= rows) {
            break;
        }
        if (map[next_pos.y][next_pos.x] == '#') {
            facing = (facing + 1) % 4;
        } else {
            pos = next_pos;
            ++path_len;
        }
    }
    return false;
}

int main() {
    FILE* input = fopen("data/day_06.txt", "r");

    int cols;
    char* raw_map = parse_input(input, &cols);
    char (*map)[cols] = (void*)raw_map;
    fclose(input);
    int rows = da_length(map) / cols;

    int pos_idx = da_indexof(map, (char) { '^' });
    Point2 start_pos = { .x = pos_idx % cols, .y = pos_idx / cols };

    Point2 pos = start_pos;
    Hset* visited = hs_create(sizeof(Point2));
    Direction facing = UP;
    int loop_cnt = 0;

    while (1) {

        hs_insert(visited, &pos);
        Point2 next_pos = p2add(p2adjacents[facing], pos);
        if (next_pos.x < 0 || next_pos.x >= cols || next_pos.y < 0 || next_pos.y >= rows) {
            break;
        }
        if (map[next_pos.y][next_pos.x] == '#') {
            facing = (facing + 1) % 4;
        } else {
            // If this is the first time we're moving to this tile, play out the scenario of an obstruction
            if (!hs_contains(*visited, &next_pos)) {
                map[next_pos.y][next_pos.x] = '#';
                loop_cnt += is_loop(rows, cols, map, pos, facing);
                map[next_pos.y][next_pos.x] = '.';
            }
            pos = next_pos;
        }
    }
    int visited_cnt = hs_length(*visited);
    hs_free(visited);

    map = NULL;
    da_free(raw_map);

    printf("Part 1: %d\n", visited_cnt);
    printf("Part 2: %d\n", loop_cnt);
}