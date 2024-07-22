#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int* parse_input(FILE* input) {
    int* seats = da_create(sizeof(*seats));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int seat = 0;
        for (int i = 0; i < 10; ++i)
            seat |= ((buffer[i] == 'B') || (buffer[i] == 'R')) << (9 - i);
        da_append(seats, seat);
    }
    return seats;
}

int int_cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int main() {
    FILE* input = fopen("data/day_05.txt", "r");
    int* seats = parse_input(input);
    fclose(input);

    int n = da_length(seats);
    qsort(seats, n, sizeof(*seats), int_cmp);
    int max_seat = seats[n - 1];
    int seat = 0;
    for (int i = 0; i < n; ++i) {
        if (seats[i] != i + seats[0]) {
            seat = i + seats[0];
            break;
        }
    }
    da_free(seats);

    printf("Part 1: %d\n", max_seat);
    printf("Part 2: %d\n", seat);
}