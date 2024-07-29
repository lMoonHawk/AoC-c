#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

typedef struct {
    char action;
    int value;
} Instruction;

static inline int mod(int a, int b) {
    return (a % b + b) % b;
}

bool next_instruction(FILE* input, Instruction* instruction) {
    char buffer[LINE_MAX] = { 0 };
    if (fgets(buffer, sizeof(buffer), input) == NULL)
        return false;
    sscanf(buffer, "%c%d", &instruction->action, &instruction->value);
    return true;
}

int main() {
    FILE* input = fopen("data/day_12.txt", "r");

    char* coords = "NESW";
    Point2 ship_p1 = { 0 };
    int facing_p1 = 1;
    Point2 ship_p2 = { 0 };
    Point2 waypoint = { .x = 10, .y = -1 };

    Instruction instruction;
    while (next_instruction(input, &instruction)) {
        char* c;
        if ((c = strchr(coords, instruction.action)) != NULL) {
            ship_p1 = p2add(ship_p1, p2scale(p2adjacents[c - coords], instruction.value));
            waypoint = p2add(waypoint, p2scale(p2adjacents[c - coords], instruction.value));
        } else if (instruction.action == 'F') {
            ship_p1 = p2add(ship_p1, p2scale(p2adjacents[facing_p1], instruction.value));
            ship_p2 = p2add(ship_p2, p2scale(waypoint, instruction.value));
        } else if (instruction.action == 'L') {
            facing_p1 = mod(facing_p1 - instruction.value / 90, 4);
            for (int i = 0; i < mod(-instruction.value / 90, 4); ++i)
                waypoint = (Point2){ .x = -waypoint.y, .y = waypoint.x };
        } else if (instruction.action == 'R') {
            facing_p1 = mod(facing_p1 + instruction.value / 90, 4);
            for (int i = 0; i < mod(instruction.value / 90, 4); ++i)
                waypoint = (Point2){ .x = -waypoint.y, .y = waypoint.x };
        }
    }
    fclose(input);
    printf("Part 1: %d\n", p2dist(ship_p1, (Point2) { 0 }));
    printf("Part 2: %d\n", p2dist(ship_p2, (Point2) { 0 }));
}