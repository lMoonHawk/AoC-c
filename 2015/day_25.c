#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

void parse_input(FILE* input, int* row, int* col) {
    char buffer[LINE_MAX];
    fgets(buffer, sizeof(buffer), input);
    *row = atoi(strstr(buffer, "row") + strlen("row") + 1);
    *col = atoi(strstr(buffer, "column") + strlen("colu;n") + 1);
}

int main() {
    FILE* input = fopen("data/day_25.txt", "r");
    int row, col;
    parse_input(input, &row, &col);

    int y = 1;
    int x = 1;
    unsigned long long code = 20151125;
    while (y != row || x != col) {
        if (y == 1) { y = x + 1; x  = 1; }
        else        { y -= 1   ; x += 1; }
        code = (code * 252533) % 33554393;
    }
    printf("Part 1: %llu\n", code);
    printf("Part 2: %s \n", "");

    fclose(input);
}