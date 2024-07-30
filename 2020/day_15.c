#include <stdio.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define ROUNDS 30000000



int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    int* prev_numbers = malloc(sizeof(int) * ROUNDS);
    memset(prev_numbers, -1, sizeof(int) * ROUNDS);

    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, LINE_MAX, input);
    int turn = 1;
    int num;
    char* tok = strtok(buffer, ",");
    while (tok != NULL) {
        num = atoi(tok);
        prev_numbers[num] = turn++;
        tok = strtok(NULL, ",");
    }
    fclose(input);

    // Last number
    int prev_number = num;
    prev_numbers[num] = -1;

    int number_p1;
    int number;
    for (int i = turn - 1; i < ROUNDS; ++i) {
        if (i == 2020)
            number_p1 = number;
        if (prev_numbers[prev_number] == -1)
            number = 0;
        else
            number = i - prev_numbers[prev_number];
        prev_numbers[prev_number] = i;
        prev_number = number;
    }
    free(prev_numbers);

    printf("Part 1: %d\n", number_p1);
    printf("Part 2: %d\n", number);
}