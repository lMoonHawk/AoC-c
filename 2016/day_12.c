#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    CPY,
    INC,
    DEC,
    JNZ,
} Operation;

typedef struct {
    Operation op;
    int args[2];
    bool isreg[2];
} Instruction;

Operation parse_op(const char* s) {
    char* ops[] = { "cpy", "inc", "dec", "jnz" };
    for (size_t i = 0; i < sizeof(ops) / sizeof(char*); ++i) {
        if (strcmp(ops[i], s) == 0)
            return i;
    }
    assert(0 && "unreachable");
    // return 0;
}

Instruction* parse_input(FILE* input) {
    Instruction* instructions = da_create(sizeof(Instruction));

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        if (buffer[0] == '\n') break;
        Instruction line = { 0 };
        // char *name = strtok(buffer, " "); 
        line.op = parse_op(strtok(buffer, " "));

        char* arg1 = strtok(NULL, " ");
        int val = atoi(arg1);
        if (val != 0) {
            line.isreg[0] = false;
            line.args[0] = atoi(arg1);
        } else {
            line.isreg[0] = true;
            line.args[0] = arg1[0] - 'a';
        }

        if (line.op != INC && line.op != DEC) {
            char* arg2 = strtok(NULL, " ");
            val = atoi(arg2);
            if (val != 0) {
                line.isreg[1] = false;
                line.args[1] = atoi(arg2);
            } else {
                line.isreg[1] = true;
                line.args[1] = arg2[0] - 'a';
            }
        }
        // printf("%d: %d (%d), %d (%d)\n", line.op, line.args[0], line.isreg[0], line.args[1], line.isreg[1]);
        da_append(instructions, line);
        // printf("%zu\n", da_length(instructions));
    }
    return instructions;
}

typedef void(*opf)(Instruction, int*, int*);

void cpy(Instruction i, int* reg, int* ip) {
    (void)ip;
    reg[i.args[1]] = i.isreg[0] ? reg[i.args[0]] : i.args[0];
}
void inc(Instruction i, int* reg, int* ip) {
    (void)ip;
    ++reg[i.args[0]];
}
void dec(Instruction i, int* reg, int* ip) {
    (void)ip;
    --reg[i.args[0]];
}
void jnz(Instruction i, int* reg, int* ip) {
    int cond = i.isreg[0] ? reg[i.args[0]] : i.args[0];
    int offset = i.isreg[1] ? reg[i.args[1]] : i.args[1];
    if (cond)
        *ip += offset - 1;
}

void interpret(Instruction* instructions, int* reg) {
    opf ops[] = { cpy, inc, dec, jnz };
    int ip = 0;
    while (ip >= 0 && ip < (int)da_length(instructions)) {
        Instruction i = instructions[ip];
        ops[i.op](i, reg, &ip);
        ++ip;
    }
}


int main() {
    FILE* input = fopen("data/day_12.txt", "r");
    Instruction* instructions = parse_input(input);
    fclose(input);

    int reg1[4] = { 0 };
    interpret(instructions, reg1);

    int reg2[4] = { 0, 0, 1, 0 };
    interpret(instructions, reg2);

    printf("Part 1: %d\n", reg1[0]);
    printf("Part 2: %d\n", reg2[0]);
    
    da_free(instructions);
}