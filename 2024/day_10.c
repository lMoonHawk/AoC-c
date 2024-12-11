#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"


uint8_t* parse_input(FILE* input, int* stride) {
    uint8_t* map = da_create(1);
    *stride = 0;
    int i = 0;
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n' && *stride == 0) {
            *stride = i;
        } else if (c != '\n' && c != '\r') {
            ++i;
            da_append(map, (uint8_t) { c - '0' });
        }
    }
    return map;
}

int get_score(int size, uint8_t map[size][size], Point2 pos, Hset* unique_nines) {
    if (map[pos.y][pos.x] == 9) {
        hs_insert(unique_nines, &pos);
        return 1;
    }
    int score = 0;
    for (int i = 0; i < 4; i++) {
        Point2 next = p2add(p2adjacents[i], pos);
        if (next.x < 0 || next.y < 0 || next.x >= size || next.y >= size)
            continue;
        if (map[next.y][next.x] != map[pos.y][pos.x] + 1)
            continue;
        score += get_score(size, map, next, unique_nines);
    }
    return score;
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");
    int size;
    uint8_t* raw_map = parse_input(input, &size);
    uint8_t(*map)[size] = (void*)raw_map;
    fclose(input);

    int th_score = 0;
    int th_rating = 0;

    for (size_t i = 0; i < da_length(raw_map); i++) {
        if (raw_map[i] == 0) {
            Point2 pos = { .x = i % size, .y = i / size };
            Hset* unique_nines = hs_create(sizeof(Point2));
            th_rating += get_score(size, map, pos, unique_nines);
            th_score += hs_length(*unique_nines);
            hs_free(unique_nines);
        }
    }
    da_free(raw_map);

    printf("Part 1: %d\n", th_score);
    printf("Part 2: %d\n", th_rating);
}