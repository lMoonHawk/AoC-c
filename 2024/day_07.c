#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

u_int64_t concatenate(u_int64_t a, u_int64_t b) {
    u_int64_t b_2 = b;
    while (b_2 != 0) {
        b_2 /= 10;
        a *= 10;
    }
    return a + b;
}

bool test_add_mul(u_int64_t result, u_int64_t* nums, size_t idx, u_int64_t val) {
    if (idx == da_length(nums))
        return result == val;
    if (val > result)
        return false;

    return test_add_mul(result, nums, idx + 1, val + nums[idx]) ||
        test_add_mul(result, nums, idx + 1, val * nums[idx]);
}

bool test_add_mul_con(u_int64_t result, u_int64_t* nums, size_t idx, u_int64_t val) {
    if (idx == da_length(nums))
        return result == val;
    if (val > result)
        return false;

    return test_add_mul_con(result, nums, idx + 1, val + nums[idx]) ||
        test_add_mul_con(result, nums, idx + 1, val * nums[idx]) ||
        test_add_mul_con(result, nums, idx + 1, concatenate(val, nums[idx]));
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");

    char buffer[LINE_MAX] = { 0 };
    u_int64_t* nums = da_create(sizeof(*nums));

    u_int64_t cnt_add_mul = 0;
    u_int64_t cnt_add_mul_con = 0;

    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* tok = strtok(buffer, ": ");
        u_int64_t result = strtoull(tok, NULL, 10);
        tok = strtok(NULL, " ");
        while (tok != NULL) {
            da_append(nums, (u_int64_t) { strtoull(tok, NULL, 10) });
            tok = strtok(NULL, " ");
        }

        if (test_add_mul(result, nums, 1, nums[0])) {
            cnt_add_mul += result;
            cnt_add_mul_con += result;
        } else if (test_add_mul_con(result, nums, 1, nums[0])) {
            cnt_add_mul_con += result;
        }

        da_set_length(nums, 0);
    }
    fclose(input);
    da_free(nums);

    printf("Part 1: %zu\n", cnt_add_mul);
    printf("Part 1: %zu\n", cnt_add_mul_con);
}