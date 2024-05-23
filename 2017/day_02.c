#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define _LINE_MAX 256

int* parse_input(FILE* input, size_t* stride) {
    int* nums = da_create(sizeof(int));
    *stride = 0;

    char buffer[_LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n') {
        size_t cnt = 0;
        char* tok = strtok(buffer, "\t");
        while (tok != NULL) {
            da_append(nums, (int) { atoi(tok) });
            tok = strtok(NULL, "\t");
            ++cnt;
        }
        if (*stride == 0) *stride = cnt;
    }
    return nums;
}

int main() {
    FILE* input = fopen("data/day_02.txt", "r");

    size_t cols;
    int* nums = parse_input(input, &cols);
    fclose(input);

    int p1 = 0, p2 = 0;

    size_t rows = da_length(nums) / cols;
    for (size_t i = 0; i < rows; ++i) {
        int min = INT_MAX, max = INT_MIN;
        for (size_t j = 0; j < cols; ++j) {
            size_t index = j + i * cols;
            if (nums[index] < min)
                min = nums[index];
            if (nums[index] > max)
                max = nums[index];

            for (size_t j2 = j + 1; j2 < cols; ++j2) {
                size_t index2 = j2 + i * cols;
                if (nums[index] % nums[index2] == 0) {
                    p2 += nums[index] / nums[index2];
                    break;
                } else if (nums[index2] % nums[index] == 0) {
                    p2 += nums[index2] / nums[index];
                    break;
                }
            }


        }
        p1 += max - min;
    }
    da_free(nums);

    printf("Part 1: %d\n", p1);
    printf("Part 2: %d\n", p2);
}