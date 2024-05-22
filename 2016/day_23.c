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
    TGL,
} Operation;

typedef struct {
    Operation op;
    int args[2];
    bool isreg[2];
} Instruction;

Operation parse_op(const char* s) {
    char* ops[] = { "cpy", "inc", "dec", "jnz", "tgl" };
    for (size_t i = 0; i < sizeof(ops) / sizeof(char*); ++i) {
        if (strcmp(ops[i], s) == 0)
            return i;
    }
    assert(0 && "unreachable");
    return 0;
}

Instruction* parse_input(FILE* input) {
    Instruction* instructions = da_create(sizeof(Instruction));

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        if (buffer[0] == '\n' || buffer[0] == '\r') break;
        Instruction line = { 0 };
        line.op = parse_op(strtok(buffer, " "));

        char* arg1 = strtok(NULL, " ");
        if (isdigit(arg1[0]) || arg1[0] == '-') {
            line.isreg[0] = false;
            line.args[0] = atoi(arg1);
        } else {
            line.isreg[0] = true;
            line.args[0] = arg1[0] - 'a';
        }

        if (line.op != INC && line.op != DEC && line.op != TGL) {
            char* arg2 = strtok(NULL, " ");
            if ((isdigit(arg2[0]) || arg2[0] == '-')) {
                line.isreg[1] = false;
                line.args[1] = atoi(arg2);
            } else {
                line.isreg[1] = true;
                line.args[1] = arg2[0] - 'a';
            }
        }
        da_append(instructions, line);
    }
    return instructions;
}

typedef void(*opf)(Instruction*, int*, int*, int);

// Returns the argument if literal or get register
int eval(Instruction ins, int arg_i, int* reg) {
    return ins.isreg[arg_i] ? reg[ins.args[arg_i]] : ins.args[arg_i];
}

void cpy(Instruction* ins, int* reg, int* ip, int ins_len) {
    (void)ins_len;
    assert(ins[*ip].isreg[1]);
    reg[ins[*ip].args[1]] = eval(ins[*ip], 0, reg);
}
void inc(Instruction* ins, int* reg, int* ip, int ins_len) {
    (void)ins_len;
    assert(ins[*ip].isreg[0]);
    ++reg[ins[*ip].args[0]];
}
void dec(Instruction* ins, int* reg, int* ip, int ins_len) {
    (void)ins_len;
    assert(ins[*ip].isreg[0]);
    --reg[ins[*ip].args[0]];
}
void jnz(Instruction* ins, int* reg, int* ip, int ins_len) {
    (void)ins_len;
    (void)ins_len;
    int cond = eval(ins[*ip], 0, reg);
    int offset = eval(ins[*ip], 1, reg);
    if (cond)
        *ip += offset - 1;
}
void tgl(Instruction* ins, int* reg, int* ip, int ins_len) {
    int target = *ip + eval(ins[*ip], 0, reg);
    if (target < 0 || target >= ins_len)
        return;
    Operation new_op = 0;
    switch (ins[target].op) {
        case CPY: new_op = JNZ; break;
        case INC: new_op = DEC; break;
        case DEC: new_op = INC; break;
        case JNZ: new_op = CPY; break;
        case TGL: new_op = INC; break;
    }
    ins[target].op = new_op;
}

void interpret(int* reg, int ins_len, Instruction instructions[ins_len]) {
    opf ops[] = { cpy, inc, dec, jnz, tgl };
    int ip = 0;
    while (ip >= 0 && ip < ins_len) {
        /*  4: cpy b c
            5: inc a
            6: dec c
            7: jnz c -2
            8: dec d
            9: jnz d -5 */
        if (ip == 4) {
            reg[0] += reg[1] * reg[3];
            reg[3] = 0;
            ip = 9;
        }
        Instruction i = instructions[ip];
        ops[i.op](instructions, reg, &ip, ins_len);
        ++ip;
    }
}

int main() {
    FILE* input = fopen("data/day_23.txt", "r");
    Instruction* instructions = parse_input(input);
    fclose(input);

    int ins_len = da_length(instructions);
    Instruction* instructions_cpy = calloc(ins_len, sizeof(Instruction));
    memcpy(instructions_cpy, instructions, ins_len * sizeof(Instruction));

    int reg1[4] = { 7, 0, 0, 0 };
    interpret(reg1, ins_len, instructions);

    int reg2[4] = { 12, 0, 0, 0 };
    interpret(reg2, ins_len, instructions_cpy);

    da_free(instructions);
    free(instructions_cpy);

    printf("Part 1: %d\n", reg1[0]);
    printf("Part 2: %d\n", reg2[0]);
}