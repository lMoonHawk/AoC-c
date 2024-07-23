#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    ACC,
    JMP,
    NOP
} Opcode;

typedef struct {
    Opcode opcode;
    int arg;
} Instruction;

Instruction* parse_input(FILE* input) {
    Instruction* program = da_create(sizeof(*program));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Instruction instruction = { 0 };
        switch (strtok(buffer, " ")[0]) {
            case 'a': instruction.opcode = ACC; break;
            case 'j': instruction.opcode = JMP; break;
            case 'n': instruction.opcode = NOP; break;
        }
        instruction.arg = atoi(strtok(NULL, " "));
        da_append(program, instruction);
    }
    return program;
}

bool interpret(Instruction* program, int* acc) {
    size_t ip = 0;
    *acc = 0;
    bool* visited = calloc(da_length(program), sizeof(bool));
    while (ip < da_length(program) && visited[ip] == false) {
        visited[ip] = true;
        if (program[ip].opcode == ACC)
            *acc += program[ip].arg;
        else if (program[ip].opcode == JMP)
            ip += program[ip].arg - 1;
        ++ip;
    }
    free(visited);
    return ip == da_length(program);
}


int main() {
    FILE* input = fopen("data/day_08.txt", "r");
    Instruction* program = parse_input(input);
    fclose(input);

    int acc_p1 = 0;
    interpret(program, &acc_p1);

    int acc_p2 = 0;
    for (size_t i = 0; i < da_length(program); ++i) {
        Opcode original = NOP, switched = JMP;
        if (program[i].opcode == JMP)
            original = JMP, switched = NOP;
        program[i].opcode = switched;
        if (interpret(program, &acc_p2))
            break;
        program[i].opcode = original;
    }
    da_free(program);

    printf("Part 1: %d\n", acc_p1);
    printf("Part 2: %d\n", acc_p2);
}