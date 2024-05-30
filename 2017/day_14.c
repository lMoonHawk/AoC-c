#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define LINE_MAX 256

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

void knot_hash_sparse(uint8_t* lengths, uint8_t sparse_hash[256]) {
    for (int k = 0; k < 256; ++k)
        sparse_hash[k] = k;

    uint8_t index = 0, skip = 0;
    for (int round = 0; round < 64; ++round) {
        for (size_t i = 0; i < da_length(lengths); ++i) {
            uint8_t end = (index + lengths[i]);
            for (int k = 0; k < lengths[i] / 2; ++k) {
                // temp index variables to avoid int promotion and control overflow of u8
                uint8_t index1 = index + k;
                uint8_t index2 = end - k - 1;
                uint8_t tmp = sparse_hash[index1];
                sparse_hash[index1] = sparse_hash[index2];
                sparse_hash[index2] = tmp;
            }
            index = index + lengths[i] + skip++;
        }
    }
}

void knot_hash_dense(uint8_t sparse_hash[256], uint8_t dense_hash[16]) {
    for (int i = 0; i < 16; ++i) {
        dense_hash[i] = 0;
        for (int j = 0; j < 16; ++j)
            dense_hash[i] ^= sparse_hash[16 * i + j];
    }
}

uint8_t* knot_hash(char* key, uint8_t* out) {
    uint8_t* lengths = da_create(sizeof(*lengths));
    char salt[] = { 17, 31, 73, 47, 23 };
    char* s = key;
    while (*s != '\0')
        da_append(lengths, (uint8_t) { *(s++) });
    for (int i = 0; i < 5; ++i)
        da_append(lengths, salt[i]);

    uint8_t sparse_hash[256] = { 0 };
    knot_hash_sparse(lengths, sparse_hash);
    knot_hash_dense(sparse_hash, out);
    da_free(lengths);
    return out;
}

bool is_used(uint8_t index, uint8_t hash[16]) {
    uint8_t byte = index / 8;
    uint8_t bit = index % 8;
    return 1 & (hash[byte] >> (8 - bit - 1));
}

bool* build_disk(FILE* input, int* used) {
    bool* disk = malloc(128 * 128);

    char key[LINE_MAX] = { 0 };
    if (!fread(key, 1, LINE_MAX, input)) exit(0);
    key[strcspn(key, "\r\n")] = '\0';
    fclose(input);

    char hash_input[LINE_MAX + 3] = { 0 };

    *used = 0;
    uint8_t hashed[16] = { 0 };
    for (uint8_t i = 0; i < 128; ++i) {
        snprintf(hash_input, LINE_MAX + 3, "%s-%d", key, i);
        knot_hash(hash_input, hashed);
        for (int j = 0; j < 128; ++j) {
            bool bit = is_used(j, hashed);
            disk[i * 128 + j] = bit;
            *used += bit;
        }
    }
    return disk;
}

void count_group(int i, int j, bool* disk) {
    disk[i * 128 + j] = false;
    int di[] = { i + 1, i - 1, i    , i };
    int dj[] = { j    , j    , j + 1, j - 1 };
    for (int k = 0; k < 4; ++k)
        if (di[k] >= 0 && di[k] < 128 && dj[k] >= 0 && dj[k] < 128 && disk[di[k] * 128 + dj[k]])
            count_group(di[k], dj[k], disk);
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");
    int used_cnt;
    bool* disk = build_disk(input, &used_cnt);

    int grps_cnt = 0;
    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 128; ++j) {
            if (disk[i * 128 + j]) {
                ++grps_cnt;
                count_group(i, j, disk);
            }
        }
    }
    free(disk);

    printf("Part 1: %d\n", used_cnt);
    printf("Part 2: %d\n", grps_cnt);
}