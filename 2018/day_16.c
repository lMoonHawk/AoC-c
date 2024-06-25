#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int opcode, a, b, c;
} Instruction;

typedef struct {
    int registers[4];
    int out_registers[4];
    Instruction instruction;
} Sample;

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

void parse_input(FILE* input, Sample** samples, Instruction** instructions) {
    *samples = da_create(sizeof(Sample));
    *instructions = da_create(sizeof(Instruction));

    bool in_sample = true;
    int sample_line = 0;

    Sample sample = { 0 };
    Instruction instruction = { 0 };

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        if (in_sample) {
            if (sample_line == 0) {
                if (buffer[0] == '\r' || buffer[0] == '\n') {
                    in_sample = false;
                    continue;
                }
                strtok(buffer, "[");
                sample.registers[0] = atoi(strtok(NULL, " "));
                sample.registers[1] = atoi(strtok(NULL, " "));
                sample.registers[2] = atoi(strtok(NULL, " "));
                sample.registers[3] = atoi(strtok(NULL, " "));
            } else if (sample_line == 1) {
                sample.instruction.opcode = atoi(strtok(buffer, " "));
                sample.instruction.a = atoi(strtok(NULL, " "));
                sample.instruction.b = atoi(strtok(NULL, " "));
                sample.instruction.c = atoi(strtok(NULL, " "));
            } else if (sample_line == 2) {
                strtok(buffer, "[");
                sample.out_registers[0] = atoi(strtok(NULL, " "));
                sample.out_registers[1] = atoi(strtok(NULL, " "));
                sample.out_registers[2] = atoi(strtok(NULL, " "));
                sample.out_registers[3] = atoi(strtok(NULL, " "));
                da_append(*samples, sample);
            } else if (sample_line == 3) {
                sample_line = -1;
            }
            ++sample_line;
        } else {
            if (buffer[0] == '\r' || buffer[0] == '\n')
                continue;
            instruction.opcode = atoi(strtok(buffer, " "));
            instruction.a = atoi(strtok(NULL, " "));
            instruction.b = atoi(strtok(NULL, " "));
            instruction.c = atoi(strtok(NULL, " "));
            da_append(*instructions, instruction);
        }
    }
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    Sample* samples = NULL;
    Instruction* instructions = NULL;
    parse_input(input, &samples, &instructions);
    fclose(input);

    opf operators[16] = { addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr };
    bool candidates[16][16]; // rows: opcode, columns: array index 

    memset(candidates, true, sizeof(candidates));
    int part1 = 0;
    int test_registers[4] = { 0 };
    for (size_t i = 0; i < da_length(samples); ++i) {
        int valid_opcodes = 0;
        for (int j = 0; j < 16; ++j) {
            memcpy(test_registers, samples[i].registers, sizeof(int) * 4);
            operators[j](samples[i].instruction.a, samples[i].instruction.b, samples[i].instruction.c, samples[i].registers, test_registers);
            if (memcmp(test_registers, samples[i].out_registers, sizeof(int) * 4) == 0) {
                ++valid_opcodes;
            } else {
                candidates[samples[i].instruction.opcode][j] = false;
            }
        }
        if (valid_opcodes >= 3) {
            ++part1;
        }
    }

    opf ord_operators[16] = { NULL };
    int index_resolved[16] = { false };
    int remaining = 16;
    while (remaining) {
        for (int i = 0; i < 16; ++i) {
            if (ord_operators[i] != NULL)
                continue;
            int possible_cnt = 0;
            int index = 0;
            for (int j = 0; j < 16; ++j) {
                if (candidates[i][j] && index_resolved[j]) {
                    candidates[i][j] = false;
                } else if (candidates[i][j]) {
                    ++possible_cnt;
                    index = j;
                }
            }
            if (possible_cnt == 1) {
                ord_operators[i] = operators[index];
                index_resolved[index] = true;
                --remaining;
                printf("%d\n", remaining);
            }
        }
    }

    int registers[4] = { 0 };
    for (size_t i = 0; i < da_length(instructions); ++i) {
        Instruction ins = instructions[i];
        ord_operators[ins.opcode](ins.a, ins.b, ins.c, registers, registers);
    }
    da_free(samples);
    da_free(instructions);

    printf("Part 1: %d\n", part1);
    printf("Part 2: %d\n", registers[0]);
}