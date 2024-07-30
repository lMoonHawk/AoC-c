#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"

#define LINE_MAX 256

bool next_instruction(FILE* input, char mask[37], int64_t* addr, int64_t* value) {
    char buffer[LINE_MAX] = { 0 };
    if (fgets(buffer, LINE_MAX, input) == NULL)
        return false;
    if (strncmp(buffer, "mask", 4) == 0) {
        strncpy(mask, buffer + 7, 36);
        fgets(buffer, LINE_MAX, input);
    }
    strtok(buffer, "[");
    *addr = atoi(strtok(NULL, "]"));
    strtok(NULL, " ");
    *value = atoi(strtok(NULL, "\r\n"));
    return true;
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");
    Hmap* mem_p1 = hm_create(sizeof(int64_t), sizeof(int64_t));
    Hmap* mem_p2 = hm_create(37, sizeof(int64_t));
    char mask_str[37] = { 0 };
    int64_t addr = 0, value = 0;
    while (next_instruction(input, mask_str, &addr, &value)) {
        int x_count = 0;

        // Part 1
        char and_mask_str[37] = { 0 };
        char or_mask_str[37] = { 0 };
        for (int i = 0; i < 36; i++) {
            and_mask_str[i] = mask_str[i];
            or_mask_str[i] = mask_str[i];
            if (mask_str[i] == 'X') {
                ++x_count;
                and_mask_str[i] = '1';
                or_mask_str[i] = '0';
            }
        }
        int64_t and_mask = strtoll(and_mask_str, NULL, 2);
        int64_t or_mask = strtoll(or_mask_str, NULL, 2);
        int64_t masked_value = (value & and_mask) | or_mask;
        hm_insert(mem_p1, &addr, &masked_value);

        // Part 2
        char floating_addr[37] = { 0 };
        for (int i = 35; i >= 0; --i) {
            floating_addr[i] = mask_str[i];
            if (mask_str[i] == '0')
                floating_addr[i] = (addr & 1) + '0';
            addr >>= 1;
        }
        int addr_count = (1 << x_count);
        for (int i = 0; i < addr_count; ++i) {
            char floating_addr_res[37] = { 0 };
            int pos = 0;
            for (int j = 0; j < 36; ++j) {
                floating_addr_res[j] = floating_addr[j];
                if (floating_addr[j] == 'X') {
                    floating_addr_res[j] = ((i >> (pos++)) & 1) + '0';
                }
            }
            hm_insert(mem_p2, floating_addr_res, &value);
        }
    }
    fclose(input);

    int64_t masked_value;

    int64_t total_p1 = 0;
    Hmap_iter iter_p1 = hm_iter(mem_p1);
    while (hm_iter_next(&iter_p1, NULL, &masked_value))
        total_p1 += masked_value;
    hm_free(mem_p1);

    int64_t total_p2 = 0;
    Hmap_iter iter_p2 = hm_iter(mem_p2);
    while (hm_iter_next(&iter_p2, NULL, &masked_value))
        total_p2 += masked_value;
    hm_free(mem_p2);

    printf("Part 1: %" PRId64 "\n", total_p1);
    printf("Part 2: %" PRId64 "\n", total_p2);
}