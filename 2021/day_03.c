#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

char** parse_input(FILE* input) {
    char** values = da_create(sizeof(char*));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        buffer[strcspn(buffer, "\n\r")] = '\0';
        da_append(values, (char*) { strdup(buffer) });
    }
    return values;
}

int bin2dec(char* binary, int bitlen) {
    int result = 0;
    for (int i = 0; i < bitlen; ++i) {
        result <<= 1;
        result += binary[i] == '1';
    }
    return result;
}

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    char** values = parse_input(input);
    int values_len = da_length(values);
    fclose(input);

    int bitlen = strlen(values[0]);
    int* counter = calloc(bitlen, sizeof(int));
    for (int i = 0; i < values_len; ++i) {
        for (int j = 0; values[i][j] != '\0'; ++j) {
            counter[j] += 2 * (values[i][j] == '1') - 1;
        }
    }
    int gamma = 0;
    for (int i = 0; i < bitlen; ++i) {
        gamma <<= 1;
        gamma += counter[i] > 0;
    }
    
    bool* oxygen = malloc(values_len * sizeof(bool));
    bool* co2 = malloc(values_len * sizeof(bool));
    memset(oxygen, 1, values_len * sizeof(bool));
    memset(co2, 1, values_len * sizeof(bool));
    int oxy_idx = 0;
    int co2_idx = 0;
    for (int col = 0; col < bitlen; ++col) {
        int oxy_value = 0;
        int co2_value = 0;
        for (int i = 0; i < values_len; ++i) {
            int value = 2 * (values[i][col] == '1') - 1;
            oxy_value += value * oxygen[i];
            co2_value += value * co2[i];
        }
        for (int i = 0; i < values_len; ++i) {
            if ((values[i][col] == '1') != (oxy_value >= 0))
                oxygen[i] = false;
            else if (oxygen[i])
                oxy_idx = i;

            if ((values[i][col] == '1') != (co2_value < 0))
                co2[i] = false;
            else if (co2[i])
                co2_idx = i;
        }
    }
    int result = bin2dec(values[oxy_idx], bitlen) * bin2dec(values[co2_idx], bitlen);
    free(oxygen);
    free(co2);

    free(counter);
    for (size_t i = 0; i < da_length(values); ++i)
        free(values[i]);
    da_free(values);

    printf("Part 1: %d\n", gamma * ((1 << bitlen) - 1 - gamma));
    printf("Part 2: %d\n", result);
}