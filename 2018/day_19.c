#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256


typedef struct {
    int opcode, a, b, c;
} Instruction;

typedef void (*opf)(int, int, int, int*, int*);

#define OP_ARGS int a, int b, int c, int* registers, int* out_registers
void addr(OP_ARGS) {
    out_registers[c] = registers[a] + registers[b];
}
void addi(OP_ARGS) {
    out_registers[c] = registers[a] + b;
}
void mulr(OP_ARGS) {
    out_registers[c] = registers[a] * registers[b];
}
void muli(OP_ARGS) {
    out_registers[c] = registers[a] * b;
}
void banr(OP_ARGS) {
    out_registers[c] = registers[a] & registers[b];
}
void bani(OP_ARGS) {
    out_registers[c] = registers[a] & b;
}
void borr(OP_ARGS) {
    out_registers[c] = registers[a] | registers[b];
}
void bori(OP_ARGS) {
    out_registers[c] = registers[a] | b;
}
void setr(OP_ARGS) {
    (void)b;
    out_registers[c] = registers[a];
}
void seti(OP_ARGS) {
    (void)b, (void)registers;
    out_registers[c] = a;
}
void gtir(OP_ARGS) {
    out_registers[c] = a > registers[b];
}
void gtri(OP_ARGS) {
    out_registers[c] = registers[a] > b;
}
void gtrr(OP_ARGS) {
    out_registers[c] = registers[a] > registers[b];
}
void eqir(OP_ARGS) {
    out_registers[c] = a == registers[b];
}
void eqri(OP_ARGS) {
    out_registers[c] = registers[a] == b;
}
void eqrr(OP_ARGS) {
    out_registers[c] = registers[a] == registers[b];
}

opf operators[16] = { addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr };
char* opnames[16] = { "addr", "addi", "mulr", "muli", "banr", "bani", "borr", "bori", "setr", "seti", "gtir", "gtri", "gtrr", "eqir", "eqri", "eqrr" };

int get_opcode(char* opname) {
    for (int i = 0; i < 16; ++i)
        if (strcmp(opnames[i], opname) == 0)
            return i;
    return -1;
}
Instruction* parse_input(FILE* input, int* bound) {
    Instruction* instructions = da_create(sizeof(Instruction));
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    strtok(buffer, " ");
    *bound = atoi(strtok(NULL, " "));
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Instruction instruction = { 0 };
        instruction.opcode = get_opcode(strtok(buffer, " "));
        instruction.a = atoi(strtok(NULL, " "));
        instruction.b = atoi(strtok(NULL, " "));
        instruction.c = atoi(strtok(NULL, " "));
        da_append(instructions, instruction);
    }
    return instructions;
}

int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    int bound = 0;
    Instruction* instructions = parse_input(input, &bound);
    fclose(input);
    int n = da_length(instructions);

    int registers[6] = { 0 };
    while (registers[bound] >= 0 && registers[bound] < n) {
        Instruction ins = instructions[registers[bound]];
        operators[ins.opcode](ins.a, ins.b, ins.c, registers, registers);
        ++registers[bound];
    }
    int part1 = registers[0];

    /*Small attempt at generalising the solution by looking at the biggest register after 100 steps.
    Through dissassembly using this specific input, one can note that the program tries to calculate
    the sum of factors of whatever is in register 4 after some amount of steps.*/
    memset(registers, 0, sizeof(registers));
    registers[0] = 1;
    for (int i = 0; i < 100; ++i) {
        Instruction ins = instructions[registers[bound]];
        operators[ins.opcode](ins.a, ins.b, ins.c, registers, registers);
        ++registers[bound];
    }
    int part2 = 0;
    int max_register_i = 0;
    for (int i = 1; i < 6; ++i)
        if (registers[i] > registers[max_register_i])
            max_register_i = i;
    for (int i = 1; i <= registers[max_register_i]; ++i)
        part2 += i * (registers[max_register_i] % i == 0);

    da_free(instructions);


    printf("Part 1: %d\n", part1);
    printf("Part 2: %d\n", part2);
}