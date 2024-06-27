#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define LINE_MAX 256

typedef struct {
    int opcode, a, b, c;
} Instruction;

typedef void (*opf)(int, int, int, int64_t*, int64_t*);

#define OP_ARGS int a, int b, int c, int64_t* registers, int64_t* out_registers
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
    FILE* input = fopen("data/day_21.txt", "r");
    int bound = 0;
    Instruction* instructions = parse_input(input, &bound);
    fclose(input);
    int n = da_length(instructions);

    /* Register 0 is only accessed once in the elfcode.
    28: eqrr 1 0 5 (r5 = r1==r0)
    29: addr 5 3 3 (if r5: r3/ip += 1 which halts)
    We run the code until we hit instruction 28 for the first time. The lowest number of instructions which halts must be when A == current B.*/
    int64_t registers[6] = { 0 };
    int64_t part1 = 0;
    while (registers[bound] >= 0 && registers[bound] < n) {
        Instruction ins = instructions[registers[bound]];
        operators[ins.opcode](ins.a, ins.b, ins.c, registers, registers);
        ++registers[bound];
        if (registers[bound] == 28) {
            part1 = registers[1];
            break;
        }
    }

    /* We run the elfcode until we see a repeat in r1 and provide the last unique r1 stored.
    The main optimisation can be implemented by noticing that the loop from instruction 17 to 27
    is an integer division of r2 with 256, which then goes back to instruction 8.*/
    int64_t part2 = 0;
    memset(registers, 0, sizeof(registers));
    Hset* seen = hs_create(sizeof(int64_t));
    while (registers[bound] >= 0 && registers[bound] < n) {
        if (registers[bound] == 17) {
            registers[2] /= 256;
            registers[bound] = 8;
            continue;
        } else if (registers[bound] == 28) {
            if (hs_contains(*seen, &registers[1]))
                break;
            hs_insert(seen, &registers[1]);
            part2 = registers[1];
        }
        Instruction ins = instructions[registers[bound]];
        operators[ins.opcode](ins.a, ins.b, ins.c, registers, registers);
        ++registers[bound];
    }
    da_free(instructions);
    hs_free(seen);
    
    printf("Part 1: %ld\n", part1);
    printf("Part 2: %ld\n", part2);

}