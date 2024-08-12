#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

uint64_t count_fishes(uint64_t timer[9]) {
    uint64_t count = 0;
    for (int i = 0; i < 9; ++i)
        count += timer[i];
    return count;
}

int main() {
    FILE* input = fopen("data/day_06.txt", "r");
    uint64_t timer[9] = { 0 };
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r') {
        ++timer[c - '0'];
        fgetc(input);
        c = fgetc(input);
    }
    fclose(input);

    uint64_t part1 = 0;
    for (int i = 0; i < 256; i++) {
        if (i == 80)
            part1 = count_fishes(timer);

        uint64_t next_timer[9] = {
            timer[1],
            timer[2],
            timer[3],
            timer[4],
            timer[5],
            timer[6],
            timer[0] + timer[7],
            timer[8],
            timer[0],
        };
        memcpy(timer, next_timer, sizeof(timer));
    }

    printf("Part 1: %" PRIu64 "\n", part1);
    printf("Part 1: %" PRIu64 "\n", count_fishes(timer));
}