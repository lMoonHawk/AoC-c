#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define _LINE_MAX 256

typedef enum { TOP, RIGHT, BOTTOM, LEFT, SIDES } Side;

typedef struct {
    int id, x, y;
    char* sides[SIDES];
    char* content;
} Tile;

bool parse_tile(FILE* input, Tile* tile, int* s_len) {
    char buffer[_LINE_MAX] = { 0 };
    if (fgets(buffer, sizeof(buffer), input) == NULL)
        return false;
    if (!sscanf(buffer, "Tile %d:", &tile->id))
        return false;
    fgets(buffer, sizeof(buffer), input);
    buffer[strcspn(buffer, "\r\n")] = '\0';
    *s_len = strlen(buffer);

    tile->content = malloc((*s_len - 2) * (*s_len - 2));
    for (Side i = 0; i < SIDES; i++)
        tile->sides[i] = malloc(*s_len);

    // Top
    memcpy(tile->sides[TOP], buffer, *s_len);
    tile->sides[RIGHT][0] = tile->sides[TOP][*s_len - 1];
    tile->sides[LEFT][0] = tile->sides[TOP][0];
    // Middle
    for (int i = 1; i < *s_len - 1; i++) {
        fgets(buffer, sizeof(buffer), input);
        tile->sides[RIGHT][i] = buffer[*s_len - 1];
        tile->sides[LEFT][i] = buffer[0];
        memcpy(tile->content + (i - 1) * (*s_len - 2), buffer + 1, *s_len - 2);
    }
    // Bottom
    fgets(buffer, sizeof(buffer), input);
    memcpy(tile->sides[BOTTOM], buffer, *s_len);
    tile->sides[RIGHT][*s_len - 1] = tile->sides[BOTTOM][*s_len - 1];
    tile->sides[LEFT][*s_len - 1] = tile->sides[BOTTOM][0];
    fgets(buffer, sizeof(buffer), input);
    return true;
}

Tile* parse_input(FILE* input, int* s_len) {
    Tile* tiles = da_create(sizeof(*tiles));
    Tile tile = { 0 };
    while (parse_tile(input, &tile, s_len))
        da_append(tiles, tile);
    return tiles;
}

void place_tile(Tile* tile, Tile ref_tile, Side side) {
    tile->x = ref_tile.x;
    tile->y = ref_tile.y;
    switch (side) {
        case TOP:    tile->y = ref_tile.y - 1; break;
        case RIGHT:  tile->x = ref_tile.x + 1; break;
        case BOTTOM: tile->y = ref_tile.y + 1; break;
        case LEFT:   tile->x = ref_tile.x - 1; break;
        case SIDES:  break;
    }
}

void swap_char(char* a, char* b) {
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

char* reverse_string(char* str, int len) {
    for (int i = 0; i < len / 2; i++)
        swap_char(str + i, str + len - i - 1);
    return str;
}

void rotate_content(char* image, int stride) {
    for (int i = 0; i < stride / 2; ++i) {
        for (int j = i; j < stride - i - 1; ++j) {
            swap_char(image + i * stride + j, image + j * stride + stride - i - 1);
            swap_char(image + i * stride + j, image + (stride - i - 1) * stride + stride - j - 1);
            swap_char(image + i * stride + j, image + (stride - j - 1) * stride + i);
        }
    }
}

void rotate_tile(Tile* tile, int s_len) {
    char* top = tile->sides[TOP];
    tile->sides[TOP] = reverse_string(tile->sides[LEFT], s_len);
    tile->sides[LEFT] = tile->sides[BOTTOM];
    tile->sides[BOTTOM] = reverse_string(tile->sides[RIGHT], s_len);
    tile->sides[RIGHT] = top;
    rotate_content(tile->content, s_len - 2);
}

void flip_content(char* image, int stride) {
    for (int i = 0; i < stride / 2; ++i) {
        for (int j = 0; j < stride; ++j) {
            char tmp = image[i * stride + j];
            image[i * stride + j] = image[(stride - i - 1) * stride + j];
            image[(stride - i - 1) * stride + j] = tmp;
        }
    }
}

void flip_tile(Tile* tile, int s_len) {
    char* top = tile->sides[TOP];
    tile->sides[TOP] = tile->sides[BOTTOM];
    tile->sides[BOTTOM] = top;
    reverse_string(tile->sides[RIGHT], s_len);
    reverse_string(tile->sides[LEFT], s_len);
    flip_content(tile->content, s_len - 2);
}

bool is_matching(char* a, Tile* b, Side side, int s_len) {
    Side b_side = (side + 2) % SIDES;

    for (int i = 0; i < 4; ++i) {
        if (memcmp(a, b->sides[b_side], s_len) == 0)
            return true;
        rotate_tile(b, s_len);
    }
    flip_tile(b, s_len);
    for (int i = 0; i < 4; ++i) {
        if (memcmp(a, b->sides[b_side], s_len) == 0)
            return true;
        rotate_tile(b, s_len);
    }
    return false;
}

int count_monsters(int stride, char(*image)[stride], char* monster, int monster_height, int monster_width) {
    int monster_count = 0;
    for (int y = 0; y < stride - monster_height; ++y) {
        for (int x = 0; x < stride - monster_width; ++x) {
            bool found = true;
            for (int i = 0; i < monster_height; ++i) {
                for (int j = 0; j < monster_width; ++j) {
                    if (monster[i * monster_width + j] == '#' && image[y + i][x + j] != '#') {
                        found = false;
                        break;
                    }
                }
                if (!found)
                    break;
            }
            monster_count += found;
        }
    }
    return monster_count;
}

int main() {
    FILE* input = fopen("data/day_20.txt", "r");
    int side_len;
    Tile* tiles = parse_input(input, &side_len);
    fclose(input);
    int tile_len = side_len - 2;

    // Place tiles starting with tiles[0] at 0,0
    int index = 0;
    Hset* placed = hs_create(sizeof(int));
    int* stack = da_create(sizeof(int));
    hs_insert(placed, &index);
    da_append(stack, index);
    while (da_length(stack)) {
        da_pop(stack, &index);
        for (Side side = 0; side < SIDES; ++side) {
            for (size_t i = 0; i < da_length(tiles); ++i) {
                if (hs_contains(*placed, &i))
                    continue;
                if (is_matching(tiles[index].sides[side], &tiles[i], side, side_len)) {
                    place_tile(&tiles[i], tiles[index], side);
                    hs_insert(placed, &i);
                    da_append(stack, i);
                    break;
                }
            }
        }
    }
    hs_free(placed);
    da_free(stack);

    // Find corners
    Tile* tl = &tiles[0];
    Tile* tr = &tiles[0];
    Tile* bl = &tiles[0];
    Tile* br = &tiles[0];
    for (size_t i = 0; i < da_length(tiles); ++i) {
        if (tiles[i].x <= tl->x && tiles[i].y <= tl->y)
            tl = &tiles[i];
        if (tiles[i].x >= tr->x && tiles[i].y <= tr->y)
            tr = &tiles[i];
        if (tiles[i].x <= bl->x && tiles[i].y >= bl->y)
            bl = &tiles[i];
        if (tiles[i].x >= br->x && tiles[i].y >= br->y)
            br = &tiles[i];
    }
    int64_t res = (int64_t)tl->id * (int64_t)tr->id * (int64_t)bl->id * (int64_t)br->id;
    
    // Collate tiles into a single image
    int tiles_per_side = 1;
    while (tiles_per_side * tiles_per_side != da_length(tiles))
        ++tiles_per_side;

    int stride = tile_len * tiles_per_side;
    char(*image)[stride] = malloc(da_length(tiles) * tile_len * tile_len);

    int image_active_count = 0;
    for (size_t ti = 0; ti < da_length(tiles); ++ti) {
        int x = tiles[ti].x - tl->x;
        int y = tiles[ti].y - tl->y;
        for (int i = 0; i < tile_len; ++i) {
            for (int j = 0; j < tile_len; ++j) {
                char sq = tiles[ti].content[i * tile_len + j];
                image[y * tile_len + i][x * tile_len + j] = sq;
                image_active_count += sq == '#';
            }
        }
    }

    // Find monster
    char* monster =
        "                  # "
        "#    ##    ##    ###"
        " #  #  #  #  #  #   ";
    int monster_height = 3;
    int monster_width = 20;
    int monster_active_count = 0;
    for (int i = 0; i < monster_height; ++i)
        for (int j = 0; j < monster_width; ++j)
            monster_active_count += monster[i * monster_width + j] == '#';

    int monster_count = 0;
    for (int i = 0; i < 4; ++i) {
        if ((monster_count = count_monsters(stride, image, monster, monster_height, monster_width)) > 0)
            break;
        rotate_content((char*)image, stride);
    }
    if (monster_count == 0) {
        flip_content((char*)image, stride);
        for (int i = 0; i < 4; ++i) {
            if ((monster_count = count_monsters(stride, image, monster, monster_height, monster_width)) > 0)
                break;
            rotate_content((char*)image, stride);
        }
    }

    // Cleanup
    free(image);
    for (size_t i = 0; i < da_length(tiles); ++i) {
        for (Side side = 0; side < SIDES; ++side)
            free(tiles[i].sides[side]);
        free(tiles[i].content);
    }
    da_free(tiles);

    printf("Part 1: %ld\n", res);
    printf("Part 2: %d\n", image_active_count - monster_active_count * monster_count);
}