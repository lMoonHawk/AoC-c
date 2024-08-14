#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

char ref[] = {
    [')'] = '(',
    [']'] = '[',
    ['}'] = '{',
    ['>'] = '<'
};
int score_corrupt[] = {
    [')'] = 3,
    [']'] = 57,
    ['}'] = 1197,
    ['>'] = 25137
};
int score_incomplete[] = {
    ['('] = 1,
    ['['] = 2,
    ['{'] = 3,
    ['<'] = 4
};

int cmp_uint64(const void* a, const void* b) {
    uint64_t a_ = *(uint64_t*)a;
    uint64_t b_ = *(uint64_t*)b;
    if (a_ > b_)
        return 1;
    else if (a_ < b_)
        return -1;
    else
        return 0;
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");

    int tot_score_corrupt = 0;
    uint64_t* tot_score_incomplete = da_create(sizeof(uint64_t));

    char* stack = da_create(1);
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        bool corrupt = false;
        for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r'; i++) {
            if (buffer[i] == '(' || buffer[i] == '[' || buffer[i] == '{' || buffer[i] == '<') {
                da_append(stack, buffer[i]);
            } else if (ref[(int)buffer[i]] == stack[da_length(stack) - 1]) {
                da_pop(stack, NULL);
            } else {
                tot_score_corrupt += score_corrupt[(int)buffer[i]];
                corrupt = true;
                break;
            }
        }
        if (!corrupt) {
            uint64_t score = 0;
            for (int i = da_length(stack) - 1; i >= 0; --i)
                score = score * 5 + score_incomplete[(int)stack[i]];
            da_append(tot_score_incomplete, score);
        }
        da_set_length(stack, 0);
    }
    fclose(input);
    da_free(stack);

    qsort(tot_score_incomplete, da_length(tot_score_incomplete), sizeof(uint64_t), cmp_uint64);

    uint64_t middle_score = tot_score_incomplete[da_length(tot_score_incomplete) / 2];
    da_free(tot_score_incomplete);

    printf("Part 1: %d\n", tot_score_corrupt);
    printf("Part 2: %" PRIu64 "\n", middle_score);
}