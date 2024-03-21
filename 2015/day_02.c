#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAYLEN(array) (sizeof((array))/sizeof((array)[0]))

int swapi(int i1, int i2, int *arr, size_t size) {
    if (i1 < 0 || i1 > (size-1) || i2 < 0 || i2 > (size-1)) return 0;
    int temp = arr[i1];
    arr[i1] = arr[i2];
    arr[i2] = temp;
    return 1;
}

int main() {
    FILE* input = fopen("2015/data/day_02.txt", "r");
    int paper = 0;
    int ribbon = 0;
    char buffer[10];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int sides[3];
        sides[0] = atoi(strtok(buffer, "x"));
        sides[1] = atoi(strtok(NULL, "x"));
        sides[2] = atoi(strtok(NULL, "x"));

        if(sides[0] > sides[1]) swapi(0, 1, sides, ARRAYLEN(sides));
        if(sides[0] > sides[2]) swapi(0, 2, sides, ARRAYLEN(sides));
        if(sides[1] > sides[2]) swapi(1, 2, sides, ARRAYLEN(sides));

        paper += 2*sides[0]*sides[1] + 2*sides[0]*sides[2] + 2*sides[1]*sides[2] + sides[0]*sides[1];
        ribbon += 2*(sides[0] + sides[1]) + sides[0]*sides[1]*sides[2];
    }
    fclose(input);
    printf("Part 1: %i\n", paper);
    printf("Part 2: %i", ribbon);
    return 0;
}

