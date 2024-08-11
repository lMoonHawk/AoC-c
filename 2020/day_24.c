#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

typedef enum { E, SE, SW, W, NW, NE, } Direction;

Point2 directions[6] = {
    {.x = 1, .y = 0 },
    {.x = 0, .y = 1 },
    {.x = -1, .y = 1 },
    {.x = -1, .y = 0 },
    {.x = 0, .y = -1 },
    {.x = 1, .y = -1 },
};

Hmap* parse_input(FILE* input) {
    Hmap* tiles = hm_create(sizeof(Point2), sizeof(bool));
    char c = fgetc(input);
    Point2 tile = { 0 };
    Point2 dir = { 0 };
    bool push = false;
    while (1) {
        if (c == '\n' || c == EOF) {
            if (!push)
                break;
            push = false;
            bool* val_ptr = hm_search(*tiles, &tile);
            if (val_ptr == NULL)
                hm_insert(tiles, &tile, &(bool){ true });
            else
                *val_ptr = 1 - *val_ptr;
            if (c == EOF)
                break;
            tile = (Point2){ .x = 0, .y = 0 };
        } else if (c != '\r') {
            if (c == 'e') {
                dir = directions[E];
            } else if (c == 'w') {
                dir = directions[W];
            } else if (c == 'n') {
                c = fgetc(input);
                if (c == 'e') {
                    dir = directions[NE];
                } else if (c == 'w') {
                    dir = directions[NW];
                }
            } else if (c == 's') {
                c = fgetc(input);
                if (c == 'e') {
                    dir = directions[SE];
                } else if (c == 'w') {
                    dir = directions[SW];
                }
            }
            tile = p2add(tile, dir);
            push = true;
        }
        c = fgetc(input);
    }
    return tiles;
}

int main() {
    FILE* input = fopen("data/day_24.txt", "r");
    Hmap* tiles = parse_input(input);
    fclose(input);

    Point2 tile;
    bool color;

    Hmap_iter iter_tiles = hm_iter(tiles);
    int count_init = 0;
    while (hm_iter_next(&iter_tiles, NULL, &color))
        count_init += color;

    Point2* to_check = da_create(sizeof(*to_check));

    for (int k = 0; k < 100; ++k) {
        Hmap* next = hm_copy(*tiles);
        iter_tiles = hm_iter(tiles);
        while (hm_iter_next(&iter_tiles, &tile, &color)) {
            int black_neigh_cnt = 0;
            for (int j = 0; j < 6; ++j) {
                Point2 neighbor = p2add(tile, directions[j]);
                bool* color_neigh_ptr = hm_search(*tiles, &neighbor);
                if (color_neigh_ptr == NULL && color) {
                    da_append(to_check, neighbor);
                } else if (color_neigh_ptr != NULL && *color_neigh_ptr) {
                    black_neigh_cnt += 1;
                }
            }
            if (color == 1 && (black_neigh_cnt == 0 || black_neigh_cnt > 2)) {
                hm_insert(next, &tile, &(bool){ false });
            } else if (color == 0 && black_neigh_cnt == 2) {
                hm_insert(next, &tile, &(bool){ true });
            }
        }

        for (size_t i = 0; i < da_length(to_check); ++i) {
            int black_neigh_cnt = 0;
            for (int j = 0; j < 6; ++j) {
                Point2 neighbor = p2add(to_check[i], directions[j]);
                bool* color_neigh_ptr = hm_search(*tiles, &neighbor);
                black_neigh_cnt += color_neigh_ptr != NULL && *color_neigh_ptr;
            }
            if (black_neigh_cnt == 2)
                hm_insert(next, &to_check[i], &(bool){ true });
        }
        da_set_length(to_check, 0);
        Hset* tmp = tiles;
        tiles = next;
        hm_free(tmp);
    }
    da_free(to_check);
    hm_free(tiles);

    int count = 0;
    iter_tiles = hm_iter(tiles);
    while (hm_iter_next(&iter_tiles, &tile, &color))
        count += color;

    printf("Part 1: %d\n", count_init);
    printf("Part 2: %d\n", count);
    
}