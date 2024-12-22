#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    ADV,
    BXL,
    BST,
    JNZ,
    BXC,
    OUT,
    BDV,
    CDV,
} Opcode;

int pow2(int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= 2;
    }
    return result;
}

uint64_t to_combo(uint8_t operand, uint64_t reg[3]) {
    if (operand <= 3) {
        return operand;
    } else if (operand != 7) {
        return reg[operand - 4];
    }
    printf("Invalid operand: %d\n", operand);
    return 0;
}

void run_program(int len, uint8_t program[len], uint64_t _reg[3], uint8_t** output) {
    uint64_t reg[3] = { _reg[0], _reg[1], _reg[2] };
    da_set_length(*output, 0);
    int ip = 0;

    while (ip < len - 1) {
        Opcode opcode = program[ip];
        uint8_t operand = program[ip + 1];
        switch (opcode) {
            case ADV:
                reg[0] = reg[0] / pow2(to_combo(operand, reg));
                break;
            case BXL:
                reg[1] = reg[1] ^ operand;
                break;
            case BST:
                reg[1] = to_combo(operand, reg) & 7;
                break;
            case JNZ:
                if (reg[0] != 0) {
                    ip = operand - 2;
                }
                break;
            case BXC:
                reg[1] = reg[1] ^ reg[2];
                break;
            case OUT:
                da_append(*output, (uint8_t) { (to_combo(operand, reg) & 7) });
                break;
            case BDV:
                reg[1] = reg[0] / pow2(to_combo(operand, reg));
                break;
            case CDV:
                reg[2] = reg[0] / pow2(to_combo(operand, reg));
                break;
        }
        ip += 2;
    }
}

uint64_t find_a(int len, uint8_t program[len], uint64_t reg[3], int depth, uint8_t** output) {
    if (depth == len) {
        return reg[0];
    }
    for (int i = 0; i < 8; i++) {
        uint64_t reg_mod[3] = { reg[0] * 8 + i, reg[1], reg[2] };
        run_program(len, program, reg_mod, output);

        if ((*output)[0] == program[len - depth - 1]) {
            uint64_t result = find_a(len, program, reg_mod, depth + 1, output);
            if (result != 0) {
                return result;
            }
        }
    }
    return 0;
}


int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    uint64_t reg[3] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    sscanf(buffer, "Register A: %"SCNu64, &reg[0]);
    fgets(buffer, sizeof(buffer), input);
    sscanf(buffer, "Register B: %"SCNu64, &reg[1]);
    fgets(buffer, sizeof(buffer), input);
    sscanf(buffer, "Register C: %"SCNu64, &reg[2]);

    uint8_t* program = da_create(sizeof(uint8_t));
    fgets(buffer, sizeof(buffer), input);
    fgets(buffer, sizeof(buffer), input);
    char* tok = strtok(buffer, " ");
    tok = strtok(NULL, ",");
    while (tok != NULL) {
        da_append(program, (uint8_t) { atoi(tok) });
        tok = strtok(NULL, ",");
    }

    uint8_t* output = da_create(sizeof(uint8_t));
    int len = da_length(program);
    run_program(len, program, reg, &output);

    char* out_buffer = calloc(2 * da_length(output), 1);
    for (size_t i = 0; i < da_length(output); i++) {
        out_buffer[2 * i] = output[i] + '0';
        out_buffer[2 * i + 1] = ',';
    }
    out_buffer[2 * da_length(output) - 1] = '\0';

    reg[0] = 0;
    uint64_t a = find_a(da_length(program), program, reg, 0, &output);

    da_free(output);
    da_free(program);

    printf("Part 1: %s\n", out_buffer);
    printf("Part 2: %"PRIu64"\n", a);
    
    free(out_buffer);
}