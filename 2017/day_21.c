#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    uint8_t stride;
    char from[9];
    char to[16];
} Rule;

void swap(char* sq, int i1, int i2) {
    char tmp = sq[i1];
    sq[i1] = sq[i2];
    sq[i2] = tmp;
}

void transpose(char* sq, uint8_t stride) {
    for (int i = 0; i < stride - 1; ++i) {
        for (int j = i + 1; j < stride; ++j) {
            swap(sq, i * stride + j, j * stride + i);
        }
    }
}

void flip(char* sq, uint8_t stride) {
    for (int i = 0; i < stride; ++i) {
        for (int j = 0; j < stride / 2; ++j) {
            swap(sq, i * stride + j, i * stride + stride - j - 1);
        }
    }
}

void cut(char* square, char(*small_squares)[16], int stride, int small_square_per_row, int small_stride) {

    for (int i = 0; i < stride; ++i) {
        for (int j = 0; j < stride; ++j) {
            int sq_k = i / small_stride * small_square_per_row + j / small_stride;
            int sq_i = i % small_stride;
            int sq_j = j % small_stride;
            small_squares[sq_k][sq_i * small_stride + sq_j] = square[i * stride + j];
        }
    }
}

void map_rule(char(*small_squares)[16], int small_square_cnt, int small_stride, Rule* rules) {
    for (int i = 0; i < small_square_cnt; ++i) {
        for (size_t r = 0; r < da_length(rules); ++r) {
            if (rules[r].stride == small_stride && memcmp(small_squares[i], rules[r].from, small_stride * small_stride) == 0) {
                strncpy(small_squares[i], rules[r].to, 16);
                break;
            }
        }
    }
}

void combine(char* square, char(*small_squares)[16], int stride, int small_stride, int small_square_per_row) {
    for (int i = 0; i < stride; ++i) {
        for (int j = 0; j < stride; ++j) {
            int sq_k = i / small_stride * small_square_per_row + j / small_stride;
            int sq_i = i % small_stride;
            int sq_j = j % small_stride;
            square[i * stride + j] = small_squares[sq_k][sq_i * small_stride + sq_j];
        }
    }
}

void parse_sq(char* s, char* out, uint8_t* stride) {
    while (*s != '\0' && *s != '\r' && *s != '\n') {
        if (*s != '/') {
            *(out++) = *s;
            if (stride != NULL)
                ++(*stride);
        } else {
            if (stride != NULL)
                *stride = 0;
        }
        ++s;
    }
}

int generate_sq(Rule* rules, char* square, int* stride, int it) {
    while (it--) {
        int small_stride = *stride % 2 == 0 ? 2 : 3;
        int small_square_per_row = *stride / small_stride;
        int small_square_cnt = small_square_per_row * small_square_per_row;

        char(*small_squares)[16] = calloc(small_square_cnt, sizeof(*small_squares));
        cut(square, small_squares, *stride, small_square_per_row, small_stride);
        map_rule(small_squares, small_square_cnt, small_stride, rules);
        *stride += small_square_per_row;
        combine(square, small_squares, *stride, small_stride + 1, small_square_per_row);
        free(small_squares);
    }
    int on_cnt = 0;
    for (size_t i = 0; i < *stride * *stride; ++i) {
        on_cnt += square[i] == '#';
    }
    return on_cnt;
}

Rule* parse_input(FILE* input) {
    Rule* rules = da_create(sizeof(*rules));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Rule rule = { 0 };
        parse_sq(strtok(buffer, " "), rule.from, &rule.stride);
        strtok(NULL, " ");
        parse_sq(strtok(NULL, " "), rule.to, NULL);
        for (int t = 0; t < 4; ++t) {
            transpose(rule.from, rule.stride);
            da_append(rules, rule);
            flip(rule.from, rule.stride);
            da_append(rules, rule);
        }
    }
    return rules;
}

int main() {
    FILE* input = fopen("data/day_21.txt", "r");
    Rule* rules = parse_input(input);
    fclose(input);

    int stride = 3;
    int max_stride = stride;
    for (int i = 0; i < 18; ++i) {
        if (max_stride % 2 == 0)
            max_stride = (max_stride * 3) / 2;
        else
            max_stride = (max_stride * 4) / 3;
    }

    char* square = calloc(max_stride * max_stride, sizeof(*square));
    memcpy(square, ".#...####", 3 * 3);

    printf("Part 1: %d\n", generate_sq(rules, square, &stride, 5));
    printf("Part 2: %d\n", generate_sq(rules, square, &stride, 13));
    da_free(rules);
    free(square);
}