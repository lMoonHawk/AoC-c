#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"

#define LINE_MAX 256

Hmap* parse_input(FILE* input) {
    Hmap* stones = hm_create(sizeof(uint64_t), sizeof(uint64_t));

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* token = strtok(buffer, " ");
        while (token != NULL) {
            hm_insert(stones, &(uint64_t) { atoi(token) }, & (uint64_t) { 1 });
            token = strtok(NULL, " ");
        }
    }
    return stones;
}

int get_digit_cnt(uint64_t num) {
    int cnt = 0;
    while (num > 0) {
        ++cnt;
        num /= 10;
    }
    return cnt;
}

void add_or_insert(Hmap* stones, uint64_t stone_val, uint64_t stone_cnt) {
    uint64_t* prev_count = hm_search(*stones, &stone_val);
    if (prev_count == NULL)
        hm_insert(stones, &stone_val, &stone_cnt);
    else
        *prev_count += stone_cnt;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");
    Hmap* stones = parse_input(input);
    fclose(input);

    uint64_t stone_val;
    uint64_t stone_cnt;
    uint64_t blinks_25;
    uint64_t blinks_75 = hs_length(*stones);

    for (int i = 0; i < 75; ++i) {
        Hmap* next_stones = hm_create(sizeof(uint64_t), sizeof(uint64_t));
        Hmap_iter it = hm_iter(stones);
        while (hm_iter_next(&it, &stone_val, &stone_cnt)) {
            int digit_cnt = get_digit_cnt(stone_val);

            if (stone_val == 0) {
                add_or_insert(next_stones, 1, stone_cnt);
            } else if (digit_cnt % 2 == 0) {
                int midpoint = 1;
                for (int i = 0; i < digit_cnt / 2; ++i)
                    midpoint *= 10;
                add_or_insert(next_stones, stone_val / midpoint, stone_cnt);
                add_or_insert(next_stones, stone_val % midpoint, stone_cnt);
                blinks_75 += stone_cnt;
                if (i == 24) 
                    blinks_25 = blinks_75;
            } else {
                add_or_insert(next_stones, stone_val * 2024, stone_cnt);
            }
        }
        hm_free(stones);
        stones = next_stones;
    }
    hm_free(stones);

    printf("Part 1: %" PRIu64 "\n", blinks_25);
    printf("Part 2: %" PRIu64 "\n", blinks_75);
}