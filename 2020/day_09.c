#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int64_t* parse_input(FILE* input) {
    int64_t* numbers = da_create(sizeof(*numbers));
    char buffer[LINE_MAX] = { 0 };
    int64_t number = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%" SCNd64, &number);
        da_append(numbers, number);
    }
    return numbers;
}

int main() {
    FILE* input = fopen("data/day_09.txt", "r");
    int64_t* numbers = parse_input(input);
    fclose(input);

    int64_t invalid_number = 0;
    for (size_t k = 25; k < da_length(numbers); ++k) {
        bool valid = false;
        for (size_t i = k - 25; i < k; ++i) {
            for (size_t j = i + 1; j < k; ++j) {
                if (numbers[i] + numbers[j] == numbers[k]) {
                    valid = true;
                    break;
                }
            }
            if (valid)
                break;
        }
        if (!valid) {
            invalid_number = numbers[k];
            break;
        }
    }

    int64_t sum, min, max;
    for (size_t i = 0; i < da_length(numbers) && sum != invalid_number; ++i) {
        sum = 0;
        min = INT64_MAX, max = INT64_MIN;
        for (size_t j = i; j < da_length(numbers) && sum < invalid_number; ++j) {
            sum += numbers[j];
            if (max < numbers[j]) max = numbers[j];
            if (min > numbers[j]) min = numbers[j];
        }
    }
    da_free(numbers);

    printf("Part 1: %" PRId64 "\n", invalid_number);
    printf("Part 2: %" PRId64 "\n", min + max);
}