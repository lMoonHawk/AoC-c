#include <stdio.h>
#include <ctype.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define LETTERS 26

void* parse_input(FILE* input) {
    char(*diagram)[LINE_MAX] = da_create(sizeof(*diagram));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        da_append(diagram, buffer[0]);
    }
    return diagram;
}

int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    char(*diagram)[LINE_MAX] = parse_input(input);
    fclose(input);

    char collected[LETTERS] = { 0 };
    char* c = collected;
    int x = strchr(diagram[0], '|') - diagram[0];
    int y = 0;
    int steps = 0;
    int mx = 0, my = 1;

    while (diagram[y][x] != ' ') {
        if (isalpha(diagram[y][x]))
            *(c++) = diagram[y][x];
        if (diagram[y][x] == '+') {
            int nmx = 1 - abs(mx), nmy = 1 - abs(my);
            if (diagram[y + nmy][x + nmx] == ' ')
                nmx = abs(mx) - 1, nmy = abs(my) - 1;
            mx = nmx, my = nmy;
        }
        x = x + mx, y = y + my;
        ++steps;
    }

    da_free(diagram);
    printf("Part 1: %s\n", collected);
    printf("Part 2: %d\n", steps);
}