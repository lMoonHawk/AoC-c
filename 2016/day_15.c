#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int pos_cnt;
    int pos_start;
} Disc;


Disc* parse_input(FILE* input) {
    Disc* discs = da_create(sizeof(Disc));

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n') {
        Disc current;
        strtok(buffer, " ");
        for (int i = 0; i < 2; ++i) strtok(NULL, " ");
        current.pos_cnt = atoi(strtok(NULL, " "));
        for (int i = 0; i < 7; ++i) strtok(NULL, " ");
        current.pos_start = atoi(strtok(NULL, " "));

        da_append(discs, current);
    }
    return discs;
}

bool timing_isvalid(Disc* discs, int t) {
    for (size_t sec = 0; sec < da_length(discs); ++sec) {
        Disc d = discs[sec];
        if ((t + d.pos_start + sec + 1) % d.pos_cnt != 0) {
            return false;
        }
    }
    return true;
}

int get_timing(Disc* discs) {
    for (int t = 0; ; ++t) if (timing_isvalid(discs, t)) return t;
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    Disc* discs = parse_input(input);
    fclose(input);

    int p1 = get_timing(discs);

    da_append(discs, ((Disc){.pos_cnt = 11, .pos_start = 0}));
    int p2 = get_timing(discs);
    da_free(discs);

    printf("Part 1: %d\n", p1);
    printf("Part 1: %d\n", p2);
}