#include <stdio.h>
#include <string.h>

#define LINE_MAX 256

typedef enum {
    FORWARD,
    DOWN,
    UP,
} Command;

int main() {
    FILE* input = fopen("data/day_02.txt", "r");

    int counter[3] = { 0 };
    int depth = 0;

    char buffer[LINE_MAX] = { 0 };
    char inst[LINE_MAX] = { 0 };
    int value;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%s %d", inst, &value);
        if (strcmp(inst, "forward") == 0) {
            counter[FORWARD] += value;
            depth += value * (counter[DOWN] - counter[UP]);
        } else if (strcmp(inst, "down") == 0) {
            counter[DOWN] += value;
        } else if (strcmp(inst, "up") == 0) {
            counter[UP] += value;
        }
    }
    fclose(input);

    printf("Part 1: %d\n", counter[FORWARD] * (counter[DOWN] - counter[UP]));
    printf("Part 1: %d\n", counter[FORWARD] * depth);
}