#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int num;
    int depth;
} Value;

typedef Value* sf_number;

sf_number* parse_input(FILE* input) {
    sf_number* numbers = da_create(sizeof(*numbers));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Value value = { .depth = -1, .num = 0 };
        bool value_buffered = false;
        sf_number number = da_create(sizeof(*number));
        for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r'; ++i) {
            if (buffer[i] == '[') {
                ++value.depth;
            } else if (buffer[i] == ']') {
                if (value_buffered) {
                    da_append(number, value);
                    value.num = 0;
                }
                value_buffered = false;
                --value.depth;
            } else if (buffer[i] == ',') {
                if (value_buffered) {
                    da_append(number, value);
                    value.num = 0;
                }
                value_buffered = false;
            } else if (buffer[i] >= '0' && buffer[i] <= '9') {
                value.num = value.num * 10 + (buffer[i] - '0');
                value_buffered = true;
            }
        }
        da_append(numbers, number);
    }
    return numbers;
}

bool explode_sf(sf_number* number) {
    for (size_t i = 0; i < da_length(*number); ++i) {
        Value value = (*number)[i];
        if (value.depth < 4)
            continue;
        if (i > 0)
            (*number)[i - 1].num += value.num;
        if (i < da_length(*number) - 2)
            (*number)[i + 2].num += (*number)[i + 1].num;
        da_remove(*number, i);
        (*number)[i] = (Value){ .num = 0, .depth = value.depth - 1 };
        return true;
    }
    return false;
}

bool split_sf(sf_number* number) {
    for (size_t i = 0; i < da_length(*number); ++i) {
        Value value = (*number)[i];
        if (value.num >= 10) {
            Value left = { .num = value.num / 2, .depth = value.depth + 1 };
            Value right = { .num = value.num - left.num, .depth = value.depth + 1 };
            (*number)[i] = right;
            da_insert(*number, left, i);
            return true;
        }
    }
    return false;
}

sf_number sum_sf(sf_number a, sf_number b) {
    sf_number result = da_create(sizeof(*result));
    for (size_t i = 0; i < da_length(a); ++i)
        da_append(result, ((Value) { a[i].num, a[i].depth + 1 }));
    for (size_t i = 0; i < da_length(b); ++i)
        da_append(result, ((Value) { b[i].num, b[i].depth + 1 }));

    while (explode_sf(&result) || split_sf(&result));
    return result;
}

int get_max_depth_sf(sf_number number) {
    int max_depth = number[0].depth;
    for (size_t i = 1; i < da_length(number); ++i)
        if (number[i].depth > max_depth)
            max_depth = number[i].depth;
    return max_depth;
}

int get_magnitude_sf(sf_number number) {
    sf_number cpy = da_copy(number);
    while (da_length(cpy) > 1) {
        int max_depth = get_max_depth_sf(cpy);
        for (size_t i = 0; i < da_length(cpy); ++i) {
            if (cpy[i].depth == max_depth) {
                Value repl = { .num = 3 * cpy[i].num + 2 * cpy[i + 1].num, .depth = cpy[i].depth - 1 };
                da_remove(cpy, i);
                cpy[i] = repl;
            }
        }
    }
    int result = cpy[0].num;
    da_free(cpy);
    return result;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");
    sf_number* sf_numbers = parse_input(input);
    fclose(input);

    sf_number sum = da_copy(sf_numbers[0]);
    for (size_t i = 1; i < da_length(sf_numbers); ++i) {
        sf_number new_sum = sum_sf(sum, sf_numbers[i]);
        da_free(sum);
        sum = new_sum;
    }
    int sum_magnitude = get_magnitude_sf(sum);
    da_free(sum);

    int max_pairwise_magnitude = 0;
    sf_number ab, ba;
    for (size_t i = 0; i < da_length(sf_numbers) - 1; ++i) {
        for (size_t j = i + 1; j < da_length(sf_numbers); ++j) {
            int mag_ab = get_magnitude_sf((ab = sum_sf(sf_numbers[i], sf_numbers[j])));
            int mag_ba = get_magnitude_sf((ba = sum_sf(sf_numbers[j], sf_numbers[i])));
            da_free(ab);
            da_free(ba);
            if (mag_ab > max_pairwise_magnitude) max_pairwise_magnitude = mag_ab;
            if (mag_ba > max_pairwise_magnitude) max_pairwise_magnitude = mag_ba;
        }
    }

    for (size_t i = 0; i < da_length(sf_numbers); ++i)
        da_free(sf_numbers[i]);
    da_free(sf_numbers);

    printf("Part 1: %d\n", sum_magnitude);
    printf("Part 2: %d\n", max_pairwise_magnitude);
}