#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"


uint64_t cs_cmprs_bytes(uint8_t* _file) {

    uint8_t* file = da_copy(_file);
    int len = da_length(file);
    uint64_t position = 0;
    uint64_t sum = 0;
    int last_block = len - 1;

    for (int first_hole = 1; first_hole < len; first_hole += 2) {
        // suming a..b positions of the fixed block, then mult by its id.
        uint64_t sum_positions = (file[first_hole - 1]) * (2 * position + file[first_hole - 1] - 1) / 2;
        sum += sum_positions * ((first_hole - 1) / 2);

        position += file[first_hole - 1];
        while (file[first_hole] > 0 && last_block > first_hole) {
            if (file[last_block] == 0) {
                last_block -= 2;
            } else {
                --file[last_block];
                --file[first_hole];
                sum += position++ * (last_block / 2);
            }
        }
    }
    da_free(file);
    return sum;
}

uint64_t cs_cmprs_blocks(uint8_t* _file) {
    uint8_t* file = da_copy(_file);

    int len = da_length(file);
    int position = 0;
    uint64_t sum = 0;

    for (int first_hole = 1; first_hole < len; first_hole += 2) {
        uint64_t sum_positions = (file[first_hole - 1]) * (2 * position + file[first_hole - 1] - 1) / 2;
        sum += sum_positions * ((first_hole - 1) / 2);
        // Skip holes made by blocks moved - need original sizes of the blocks
        position += _file[first_hole - 1];

        for (int block = len - 1; block >= first_hole; block -= 2) {
            if (file[first_hole] == 0)
                break;
            if (file[block] == 0)
                continue;
            if (file[block] <= file[first_hole]) {
                uint64_t sumpos = (file[block]) * (2 * position + file[block] - 1) / 2;
                sum += sumpos * (block / 2);
                position += file[block];
                file[first_hole] -= file[block];
                file[block] = 0;
            }
        }
        position += file[first_hole];
    }
    da_free(file);
    return sum;
}



int main() {
    FILE* input = fopen("data/day_09.txt", "r");
    uint8_t* file = da_create(sizeof(*file));
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r') {
        da_append(file, (uint8_t) { c - '0' });
        c = fgetc(input);
    }
    fclose(input);

    uint64_t cs_cmprs_bytes_val = cs_cmprs_bytes(file);
    uint64_t cs_cmprs_blocks_val = cs_cmprs_blocks(file);
    da_free(file);

    printf("Part 1: %" PRIu64 "\n", cs_cmprs_bytes_val);
    printf("Part 2: %" PRIu64 "\n", cs_cmprs_blocks_val);
}