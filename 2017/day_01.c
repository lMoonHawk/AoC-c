#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

int main() {
    FILE* input = fopen("data/day_01.txt", "r");
    char* digits = da_create(sizeof(char));

    char c;
    while ((c = fgetc(input)) != EOF && c != '\r' && c != '\n')
        da_append(digits, (char) { c - '0' });
    fclose(input);

    int sum1 = 0, sum2 = 0;

    size_t n = da_length(digits);
    for (size_t i = 0; i < n; ++i) {
        size_t next_i1 = (i + 1) % n;
        if (digits[i] == digits[next_i1])
            sum1 += digits[i];
        size_t next_i2 = (i + n / 2) % n;
        if (digits[i] == digits[next_i2])
            sum2 += digits[i];
    }
    da_free(digits);

    printf("Part 1: %d\n", sum1);
    printf("Part 2: %d\n", sum2);
}