#include <stdio.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"

typedef struct {
    char type;
    Point2 pos;
} Antenna;

Antenna* parse_input(FILE* input, P2Bounds* bounds) {
    Antenna* antennas = da_create(sizeof(Antenna));
    *bounds = (P2Bounds){ 0 };
    int x = 0;
    int y = 0;
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (c != '\n' && c != '\r') {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                da_append(antennas, ((Antenna) {.type = c, .pos = { .x = x, .y = y } }));
            }
            ++x;
            bounds->max.y = y;
        } else if (c == '\n') {
            ++y;
            if (bounds->max.x == 0)
                bounds->max.x = x - 1;
            x = 0;
        }
    }
    return antennas;
}


int main() {
    FILE* input = fopen("data/day_08.txt", "r");
    P2Bounds bounds;
    Antenna* antennas = parse_input(input, &bounds);
    fclose(input);

    int antenna_cnt = da_length(antennas);
    Hset* antinodes = hs_create(sizeof(Point2));
    Hset* antinodes_harmonics = hs_create(sizeof(Point2));
    for (int i = 0; i < antenna_cnt; ++i) {
        for (int j = i + 1; j < antenna_cnt; ++j) {

            if (antennas[i].type != antennas[j].type)
                continue;

            int diff_x = antennas[i].pos.x - antennas[j].pos.x;
            int diff_y = antennas[i].pos.y - antennas[j].pos.y;

            Point2 an1 = antennas[i].pos;
            while (p2in_bounds(an1, bounds)) {
                hs_insert(antinodes_harmonics, &an1);
                an1.x += diff_x;
                an1.y += diff_y;
            }
            an1 = (Point2){ .x = antennas[i].pos.x + diff_x, .y = antennas[i].pos.y + diff_y };
            if (p2in_bounds(an1, bounds))
                hs_insert(antinodes, &an1);

            Point2 an2 = antennas[j].pos;
            while (p2in_bounds(an2, bounds)) {
                hs_insert(antinodes_harmonics, &an2);
                an2.x -= diff_x;
                an2.y -= diff_y;
            }
            an2 = (Point2){ .x = antennas[j].pos.x - diff_x, .y = antennas[j].pos.y - diff_y };
            if (p2in_bounds(an2, bounds))
                hs_insert(antinodes, &an2);

        }
    }
    da_free(antennas);

    int antinodes_cnt = hs_length(*antinodes);
    int antinodes_harmonics_cnt = hs_length(*antinodes_harmonics);
    hs_free(antinodes);
    hs_free(antinodes_harmonics);

    printf("Part 1: %d\n", antinodes_cnt);
    printf("Part 2: %d\n", antinodes_harmonics_cnt);
}