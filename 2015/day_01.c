#include <stdio.h>

int main() {
    FILE* input = fopen("data/day_01.txt", "r");
    char c;
    int floor = 0;
    int position = 0;
    int basement = 0;
    while ((c = fgetc(input)) != EOF) {
        position++;
        floor += (c == '(') ? 1 : -1;
        if (floor == -1 && basement == 0) basement = position;
    }
    fclose(input);
    printf("Part 1: %i\n", floor);
    printf("Part 2: %i", basement);
    return 0;
}
