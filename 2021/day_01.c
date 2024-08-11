#include <stdio.h>
#include <stdlib.h>

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_01.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    int window[3] = { 0 };
    int n = 0;
    int diff1 = 0, diff2 = 0;
    while (fgets(buffer, LINE_MAX, input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int num = atoi(buffer);
        if (n > 0)
            diff1 += num - window[0] > 0;
        if (n > 2)
            diff2 += num - window[2] > 0;
        window[2] = window[1];
        window[1] = window[0];
        window[0] = num;
        ++n;
    }
    fclose(input);

    printf("Part 1: %d\n", diff1);
    printf("Part 2: %d\n", diff2);
}