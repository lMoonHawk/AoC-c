#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

int interpret(int* program) {
    int ip = 0;
    while (1) {
        int op = program[ip];
        switch (op) {
            case 1:
                program[program[ip + 3]] = program[program[ip + 1]] + program[program[ip + 2]];
                break;
            case 2:
                program[program[ip + 3]] = program[program[ip + 1]] * program[program[ip + 2]];
                break;
            case 99:
                return program[0];
        }
        ip += 4;
    }
}

int* parse_input(FILE* input) {
    int* program = da_create(sizeof(*program));
    char c = fgetc(input);
    int buffer = 0;
    while (c != '\n' && c != '\r' && c != EOF) {
        if (c == ',') {
            da_append(program, buffer);
            buffer = 0;
        } else {
            buffer = buffer * 10 + c - '0';
        }
        c = fgetc(input);
    }
    da_append(program, buffer);
    return program;
}

int main() {
    FILE* input = fopen("data/day_02.txt", "r");
    int* program = parse_input(input);
    fclose(input);

    int* program_cpy = malloc(sizeof(int) * da_length(program));
    memcpy(program_cpy, program, sizeof(int) * da_length(program));

    program_cpy[1] = 12;
    program_cpy[2] = 2;
    int part1 = interpret(program_cpy);
    memcpy(program_cpy, program, sizeof(int) * da_length(program));

    int part2 = 0;
    for (int nounverb = 0; nounverb < 100 * 100; ++nounverb) {
        int noun = nounverb / 100;
        int verb = nounverb % 100;
        program_cpy[1] = noun;
        program_cpy[2] = verb;
        if (interpret(program_cpy) == 19690720) {
            part2 = 100 * noun + verb;
            break;
        }
        memcpy(program_cpy, program, sizeof(int) * da_length(program));
    }
    da_free(program);
    free(program_cpy);

    printf("%d\n", part1);
    printf("%d\n", part2);
}