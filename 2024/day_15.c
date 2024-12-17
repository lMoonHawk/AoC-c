#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define _LINE_MAX 256

typedef enum {
    ROBOT,
    BOX,
    BOX_LEFT,
    BOX_RIGHT,
    WALL,
    EMPTY,
} Tile;

typedef enum {
    UP, RIGHT, DOWN, LEFT,
} Direction;

Tile* parse_input(FILE* input, int* stride, Point2* robot_pos) {
    Tile* map = da_create(sizeof(Tile));
    char buffer[_LINE_MAX] = { 0 };
    *stride = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        if (!*stride)
            *stride = strcspn(buffer, "\r\n");
        for (int i = 0; i < *stride; i++) {
            if (buffer[i] == '@') {
                *robot_pos = (Point2){ .x = i, .y = da_length(map) / (*stride) };
                da_append(map, (Tile) { ROBOT });
            } if (buffer[i] == 'O') {
                da_append(map, (Tile) { BOX });
            } else if (buffer[i] == '#') {
                da_append(map, (Tile) { WALL });
            } else if (buffer[i] == '.') {
                da_append(map, (Tile) { EMPTY });
            }
        }
    }
    return map;
}

Tile* widen_warehouse(Tile* map, int len) {
    Tile* w_map = malloc(sizeof(Tile) * len * 2);
    for (int i = 0; i < len; i++) {
        if (map[i] == BOX) {
            w_map[i * 2] = BOX_LEFT;
            w_map[i * 2 + 1] = BOX_RIGHT;
        } else if (map[i] == ROBOT) {
            w_map[i * 2] = ROBOT;
            w_map[i * 2 + 1] = EMPTY;
        } else {
            w_map[i * 2] = map[i];
            w_map[i * 2 + 1] = map[i];
        }
    }
    return w_map;
}

static inline bool is_vertical(Direction direction) {
    return direction == UP || direction == DOWN;
}

bool can_move(Tile* map, int stride, Point2 pos, int direction) {
    Point2 new_pos = p2add(pos, p2adjacents[direction]);
    int new_idx = new_pos.y * stride + new_pos.x;
    Tile next_tile = map[new_idx];

    if (next_tile == EMPTY) {
        return true;
    } else if (next_tile == BOX_LEFT && is_vertical(direction)) {
        return can_move(map, stride, new_pos, direction) &&
            can_move(map, stride, p2add(new_pos, p2adjacents[RIGHT]), direction);
    } else if (next_tile == BOX_RIGHT && is_vertical(direction)) {
        return can_move(map, stride, p2add(new_pos, p2adjacents[LEFT]), direction) &&
            can_move(map, stride, new_pos, direction);
    } else if (next_tile == BOX || next_tile == BOX_LEFT || next_tile == BOX_RIGHT) {
        return can_move(map, stride, new_pos, direction);
    }
    return false;
}

Point2 move(Tile* map, int stride, Point2 pos, int direction) {
    Point2 new_pos = p2add(pos, p2adjacents[direction]);
    int idx = pos.y * stride + pos.x;
    int new_idx = new_pos.y * stride + new_pos.x;
    Tile next_tile = map[new_idx];

    if (next_tile == BOX_LEFT && is_vertical(direction)) {
        move(map, stride, new_pos, direction);
        move(map, stride, p2add(new_pos, p2adjacents[RIGHT]), direction);
    } else if (next_tile == BOX_RIGHT && is_vertical(direction)) {
        move(map, stride, new_pos, direction);
        move(map, stride, p2add(new_pos, p2adjacents[LEFT]), direction);
    } else if (next_tile == BOX || next_tile == BOX_LEFT || next_tile == BOX_RIGHT) {
        move(map, stride, new_pos, direction);
    }
    Tile obj = map[idx];
    map[idx] = EMPTY;
    map[new_idx] = obj;
    return new_pos;
}

int get_gps(Tile* map, int rows, int cols) {
    int gps = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i * cols + j] == BOX || map[i * cols + j] == BOX_LEFT) {
                gps += 100 * i + j;
            }
        }
    }
    return gps;
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    int stride;
    Point2 pos_p1;
    Tile* map_p1 = parse_input(input, &stride, &pos_p1);
    Tile* map_p2 = widen_warehouse(map_p1, da_length(map_p1));
    Point2 pos_p2 = (Point2){ .x = pos_p1.x * 2, .y = pos_p1.y };

    const char* directions = "^>v<";
    char c;
    while ((c = fgetc(input)) != EOF) {
        char* e = strchr(directions, c);
        Direction direction = e - directions;
        if (e != NULL) {
            if (can_move(map_p1, stride, pos_p1, direction))
                pos_p1 = move(map_p1, stride, pos_p1, direction);
            if (can_move(map_p2, stride * 2, pos_p2, direction))
                pos_p2 = move(map_p2, stride * 2, pos_p2, direction);
        }
    }
    fclose(input);

    int rows = da_length(map_p1) / stride;
    int sum_gps_p1 = get_gps(map_p1, rows, stride);
    int sum_gps_p2 = get_gps(map_p2, rows, stride * 2);
    da_free(map_p1);
    free(map_p2);

    printf("Part 1: %d\n", sum_gps_p1);
    printf("Part 2: %d\n", sum_gps_p2);
}