#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    SWAP_POS,
    SWAP_LETTERS,
    ROTATE_STEPS,
    ROTATE_POS,
    REVERSE_POS,
    MOVE_POS,
} Operation;

typedef union {
    struct { int i1, i2; } swap_pos;
    struct { char c1, c2; } swap_letters;
    struct { int steps; } rotate_steps;
    struct { char c; } rotate_pos;
    struct { int i1, i2; } reverse_pos;
    struct { int from, to; } move_pos;
} Args;

typedef struct {
    Operation op;
    Args args;
} Instruction;

Instruction parse_swap_pos(char* line) {
    // swap position 6 with position 5
    Instruction i = { .op = SWAP_POS };
    strtok(line, " ");
    strtok(NULL, " ");
    i.args.swap_pos.i1 = atoi(strtok(NULL, " "));
    strtok(NULL, " ");
    strtok(NULL, " ");
    i.args.swap_pos.i2 = atoi(strtok(NULL, " "));
    return i;
}
Instruction parse_swap_letters(char* line) {
    // swap letter b with letter d
    Instruction i = { .op = SWAP_LETTERS };
    strtok(line, " ");
    strtok(NULL, " ");
    i.args.swap_letters.c1 = *strtok(NULL, " ");
    strtok(NULL, " ");
    strtok(NULL, " ");
    i.args.swap_letters.c2 = *strtok(NULL, " ");
    return i;
}
Instruction parse_rotate_steps(char* line) {
    // rotate right 4 steps
    Instruction i = { .op = ROTATE_STEPS };
    int direction = 1;
    strtok(line, " ");
    if (strcmp("left", strtok(NULL, " ")) == 0)
        direction = -1;
    i.args.rotate_steps.steps = direction * atoi(strtok(NULL, " "));
    return i;
}
Instruction parse_rotate_pos(char* line) {
    // rotate based on position of letter e
    Instruction ins = { .op = ROTATE_POS };
    strtok(line, " ");
    for (int i = 0; i < 5; ++i) strtok(NULL, " ");
    ins.args.rotate_pos.c = *strtok(NULL, " ");
    return ins;
}
Instruction parse_reverse_pos(char* line) {
    // reverse positions 2 through 7
    Instruction ins = { .op = REVERSE_POS };
    strtok(line, " ");
    strtok(NULL, " ");
    ins.args.reverse_pos.i1 = atoi(strtok(NULL, " "));
    strtok(NULL, " ");
    ins.args.reverse_pos.i2 = atoi(strtok(NULL, " "));
    return ins;
}
Instruction parse_move_pos(char* line) {
    // move position 5 to position 0
    Instruction ins = { .op = MOVE_POS };
    strtok(line, " ");
    strtok(NULL, " ");
    ins.args.move_pos.from = atoi(strtok(NULL, " "));
    strtok(NULL, " ");
    strtok(NULL, " ");
    ins.args.move_pos.to = atoi(strtok(NULL, " "));
    return ins;
}
void swap_pos(char* s, int i1, int i2) {
    char tmp = s[i1];
    s[i1] = s[i2];
    s[i2] = tmp;
}

void swap_letters(char* s, char c1, char c2) {
    while (*s) {
        if (*s == c1) *s = c2;
        else if (*s == c2) *s = c1;
        s++;
    }
}

void rotate_steps(char* s, int steps, int size) {
    steps = steps % size;
    if (steps < 0)
        steps = size + steps;

    char temp[steps];
    strncpy(temp, s + size - steps, steps);
    memmove(s + steps, s, size - steps);
    strncpy(s, temp, steps);
}

void rotate_pos(char* s, char c, int size) {
    int index;
    for (index = 0; s[index] != c && index < size; ++index);
    rotate_steps(s, 1 + index + (index >= 4), size);
}

void reverse_pos(char* s, int i1, int i2) {
    int n = (i2 - i1 + 1) / 2;
    for (int i = 0; i < n; ++i) {
        char tmp = s[i + i1];
        s[i1 + i] = s[i2 - i];
        s[i2 - i] = tmp;
    }
}

void move_pos(char* s, int from, int to, int size) {
    char tmp = s[from];
    memmove(s + from, s + from + 1, size - from - 1);
    memmove(s + to + 1, s + to, size - to - 1);
    s[to] = tmp;
}

void rev_rotate_pos(char* s, char c, int size) {
    for (int i = 0; i < size; ++i) {
        rotate_steps(s, -1, size);
        if (i == 4)
            rotate_steps(s, -1, size);
        if (s[i] == c)
            break;
    }
}

Instruction parse_instruction(char* line) {
    if (strstr(line, "swap") != NULL) {
        if (strstr(line, "position") != NULL)
            return parse_swap_pos(line);
        return parse_swap_letters(line);
    }
    if (strstr(line, "rotate") != NULL) {
        if (strstr(line, "based") != NULL)
            return parse_rotate_pos(line);
        return parse_rotate_steps(line);
    }
    if (strstr(line, "reverse") != NULL)
        return parse_reverse_pos(line);
    return parse_move_pos(line);
}

Instruction* parse_input(FILE* input) {
    Instruction* instructions = da_create(sizeof(Instruction));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n') {
        Instruction ins = parse_instruction(buffer);
        da_append(instructions, ins);
    }
    return instructions;
}

void run(char* pw, int size, Instruction* instructions, bool reverse) {
    size_t len = da_length(instructions);
    for (size_t i = 0; i < len; ++i) {
        Instruction ins;
        if (reverse)
            ins = instructions[len - i - 1];
        else
            ins = instructions[i];

        switch (ins.op) {
            case SWAP_POS:
                swap_pos(pw, ins.args.swap_pos.i1, ins.args.swap_pos.i2);
                break;
            case SWAP_LETTERS:
                swap_letters(pw, ins.args.swap_letters.c1, ins.args.swap_letters.c2);
                break;
            case ROTATE_STEPS:
                if (reverse)
                    rotate_steps(pw, -ins.args.rotate_steps.steps, size);
                else
                    rotate_steps(pw, ins.args.rotate_steps.steps, size);
                break;
            case ROTATE_POS:
                if (reverse)
                    rev_rotate_pos(pw, ins.args.rotate_pos.c, size);
                else
                    rotate_pos(pw, ins.args.rotate_pos.c, size);
                break;
            case REVERSE_POS:
                reverse_pos(pw, ins.args.reverse_pos.i1, ins.args.reverse_pos.i2);
                break;
            case MOVE_POS:
                if (reverse)
                    move_pos(pw, ins.args.move_pos.to, ins.args.move_pos.from, size);
                else
                    move_pos(pw, ins.args.move_pos.from, ins.args.move_pos.to, size);
                break;
        }
    }
}

int main() {
    FILE* input = fopen("data/day_21.txt", "r");
    Instruction* instructions = parse_input(input);
    fclose(input);

    char pw1[] = "abcdefgh";
    char pw2[] = "fbgdceah";
    run(pw1, strlen(pw1), instructions, false);
    run(pw2, strlen(pw2), instructions, true);

    printf("Part 1: %s\n", pw1);
    printf("Part 2: %s\n", pw2);
}