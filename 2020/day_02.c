#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_02.txt", "r");

    int p1_count = 0, p2_count = 0;
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int min = atoi(strtok(buffer, "-"));
        int max = atoi(strtok(NULL, " "));
        char letter = strtok(NULL, ":")[0];
        char* s = strtok(NULL, " \r\n");
        int cnt = 0, pos = 0;
        for (int i = 0; s[i] != '\0'; ++i) {
            cnt += (s[i] == letter);
            pos += (s[i] == letter) && (min == i + 1 || max == i + 1);
        }
        p1_count += (cnt >= min && cnt <= max);
        p2_count += (pos == 1);
    }
    fclose(input);

    printf("Part 1: %d\n", p1_count);
    printf("Part 2: %d\n", p2_count);
}