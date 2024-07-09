#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

static inline int mod(int a, int b) {
    return (a % b + b) % b;
}

uint8_t get_color(Hmap* colors, Point2 position) {
    uint8_t* color = hm_search(*colors, &position);
    if (color == NULL)
        return false;
    return *color;
}

Hmap* paint_ship(int64_t* program, int first_color, int* painted) {
    Point2 directions[4] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

    IC_state* state = ic_create(program);
    Point2 position = { 0, 0 };
    int facing = 0;
    Hmap* colors = hm_create(sizeof(Point2), sizeof(uint8_t));
    ic_push_input(state, first_color);
    while (true) {
        if (ic_interpret(state) == IC_HALT)
            break;
        uint8_t color = ic_pop_output(state);
        uint8_t* color_ptr = hm_search(*colors, &position);
        if (color_ptr == NULL) {
            if (painted != NULL)
                ++*painted;
            hm_insert(colors, &position, &color);
        } else {
            *color_ptr = color;
        }
        uint8_t turn = ic_pop_output(state);
        facing = mod(facing + (2 * turn - 1), 4);
        position = p2add(position, directions[facing]);
        ic_push_input(state, get_color(colors, position));
    }
    ic_free(state);
    return colors;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    int painted = 0;
    hm_free(paint_ship(program, 0, &painted));
    
    Hmap* ship = paint_ship(program, 1, NULL);
    da_free(program);

    P2Bounds bounds = { {INT_MAX, INT_MAX}, {INT_MIN, INT_MIN} };
    for (size_t i = 0; i < ship->capacity; ++i) {
        if (ship->htable[i].key != NULL && !ship->htable[i].deleted && *(uint8_t*)ship->htable[i].value == 1) {
            Point2 p = *(Point2*)ship->htable[i].key;
            if (bounds.min.x > p.x) bounds.min.x = p.x;
            if (bounds.min.y > p.y) bounds.min.y = p.y;
            if (bounds.max.x < p.x) bounds.max.x = p.x;
            if (bounds.max.y < p.y) bounds.max.y = p.y;
        }
    }

    int buffer_cols = (bounds.max.x - bounds.min.x + 2);
    int buffer_rows = (bounds.max.y - bounds.min.y + 1);
    char* buffer = malloc(buffer_cols * buffer_rows);
    for (int y = 0; y < bounds.max.y - bounds.min.y + 1; ++y) {
        for (int x = 0; x < bounds.max.x - bounds.min.x + 2; ++x) {
            Point2 p = { x + bounds.min.x, y + bounds.min.y };
            uint8_t* color = hm_search(*ship, &p);
            if (color == NULL || *color == 0)
                buffer[y * buffer_cols + x] = ' ';
            else
                buffer[y * buffer_cols + x] = '#';
        }
        buffer[y * buffer_cols + buffer_cols - 1] = '\n';
    }
    buffer[buffer_cols * buffer_rows - 1] = '\0';
    hm_free(ship);


    printf("Part 1: %d\n", painted);
    printf("Part 2: \n%s\n", buffer);
    
    free(buffer);
}