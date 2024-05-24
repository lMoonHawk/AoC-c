#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int* parse_input(FILE* input, int* n) {
    int* nums = da_create(sizeof(*nums));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        da_append(nums, (int) { atoi(buffer) });
        ++*n;
    }
    return nums;
}

int main() {
    FILE* input = fopen("data/day_05.txt", "r");
    int n = 0;
    int* nums = parse_input(input, &n);
    fclose(input);

    int* nums_cpy = malloc(n * sizeof(int));
    memcpy(nums_cpy, nums, n*sizeof(int));

    int ip = 0, cnt = 0;
    for (cnt = 0; ip >= 0 && ip < n; ++cnt)
        ip += nums[ip]++;
    da_free(nums);

    int ip2 = 0, cnt2 = 0;
    for (cnt2 = 0; ip2 >= 0 && ip2 < n; ++cnt2) {
        int inc = nums_cpy[ip2] >= 3 ? -1 : 1;
        nums_cpy[ip2] += inc;
        ip2 += nums_cpy[ip2] - inc;
    }
    free(nums_cpy);

    printf("Part 1: %d\n", cnt);
    printf("Part 2: %d\n", cnt2);
}