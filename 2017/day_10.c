#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LIST_SIZE 256
#define LINE_MAX 256

int* knot_hash(int* lengths, int count) {

    int* list = malloc(LIST_SIZE * sizeof(*list));
    for (int k = 0; k < LIST_SIZE; ++k)
        list[k] = k;

    int index = 0, skip = 0;

    for (int round = 0; round < count; ++round) {
        for (size_t i = 0; i < da_length(lengths); ++i) {
            int start = index % LIST_SIZE;
            int end = (index + lengths[i]) % LIST_SIZE;

            index += lengths[i] + skip++;
            for (int k = 0; k < lengths[i] / 2; ++k) {
                int index1 = (start + k) % LIST_SIZE;
                int index2 = (end - k - 1) % LIST_SIZE;
                if (index2 < 0)
                    index2 += LIST_SIZE;

                int tmp = list[index1];
                list[index1] = list[index2];
                list[index2] = tmp;
            }
        }
    }
    return list;
}

// Pass the nibbles as hex characters in out.
void hexdigest(int hash[16], char out[33]) {
    for (int i = 0; i < 16; ++i) {
        snprintf(out, 3, "%02x", hash[i]);
        out += 2;
    }
}

void parse_input(FILE* input, int** lengths, int** lengths_ascii) {
    char c = fgetc(input);
    int length = 0;
    while (c != '\0' && c != '\r' && c != '\n') {
        da_append(*lengths_ascii, (int) { c });
        if (c == ',') {
            da_append(*lengths, length);
            length = 0;
        } else {
            length = length * 10 + (c - '0');
        }
        c = fgetc(input);
    }
    da_append(*lengths, length);
    int add[] = { 17, 31, 73, 47, 23 };
    for (int i = 0; i < 5; ++i) {
        da_append(*lengths_ascii, (int) { add[i] });
    }
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");

    int* lengths = da_create(sizeof(*lengths));
    int* lengths_ascii = da_create(sizeof(*lengths_ascii));
    parse_input(input, &lengths, &lengths_ascii);

    int* list = knot_hash(lengths, 1);
    int p1 = list[0] * list[1];
    da_free(lengths);
    free(list);

    int* sparse_hash = knot_hash(lengths_ascii, 64);
    da_free(lengths_ascii);
    int dense_hash[16] = { 0 };
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 16; ++j)
            dense_hash[i] ^= sparse_hash[16 * i + j];
    free(sparse_hash);

    char dense_hash_repr[16 * 2 + 1] = {0};
    hexdigest(dense_hash, dense_hash_repr);
    
    printf("Part 1: %d\n", p1);
    printf("Part 2: %s\n", dense_hash_repr);
}