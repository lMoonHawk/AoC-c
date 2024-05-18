#include <stdio.h>
#include <stdlib.h>

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);
    int num = atoi(buffer);

    // https://en.wikipedia.org/wiki/Josephus_problem#Solution
    int p = 1;
    while (p <= num) p *= 2;
    p = (2 * num) - p + 1;

    // Get winner for n using n-1
    // Winner of round i+1 is next index if before the index of the gift, else next+1 since that number left a gap.
    int index = 1;
    for (int i = 1; i < num; ++i) {
        index = index % i + 1;
        if (index > (i + 1) / 2)
            index += 1;
    }

    printf("Part 1: %d\n", p);
    printf("Part 2: %d\n", index);

}