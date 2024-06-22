#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define LINE_MAX 256

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

uint64_t parse_end(uint8_t* scoreboard, size_t len) {
    size_t n = da_length(scoreboard);
    if (n < len)
        return 0;
    uint64_t out = 0;
    for (size_t i = n - len; i < n; ++i)
        out = out * 10 + scoreboard[i];
    return out;
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);

    size_t len_end = strlen(buffer);
    size_t end = atoi(buffer);
    uint64_t last_10 = 0;

    uint8_t* scoreboard = da_create(sizeof(*scoreboard));
    da_append(scoreboard, (uint8_t) { 3 });
    da_append(scoreboard, (uint8_t) { 7 });
    size_t e1 = 0, e2 = 1;
    while (1) {
        uint8_t score_e1 = scoreboard[e1];
        uint8_t score_e2 = scoreboard[e2];
        uint8_t new_score = score_e1 + score_e2;
        uint8_t new_score_1 = (new_score / 10) % 10;
        uint8_t new_score_2 = new_score % 10;
        if (new_score_1 != 0) {
            da_append(scoreboard, new_score_1);
            if (parse_end(scoreboard, len_end) == end)
                break;
        }
        da_append(scoreboard, new_score_2);
        if (parse_end(scoreboard, len_end) == end)
            break;
        e1 = (e1 + score_e1 + 1) % da_length(scoreboard);
        e2 = (e2 + score_e2 + 1) % da_length(scoreboard);

        if (last_10 == 0 && da_length(scoreboard) >= end + 10) {
             last_10 = parse_end(scoreboard, 10);
        }
    }

    printf("Part 1: %lu\n", last_10);
    printf("Part 1: %zu\n", da_length(scoreboard) - len_end);
    da_free(scoreboard);
}
