#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

char str_ops[6][4] = { "hlf", "tpl", "inc", "jmp", "jio", "jie" };
typedef enum {
    HLF, TPL, INC, JMP, JIO, JIE,
    OP_CNT,
} Op;

Op get_operation(const char* sop) {
    for (Op i = 0; i < OP_CNT; ++i) if (strcmp(str_ops[i], sop) == 0) return i;
    return OP_CNT;
}

void parse_input(FILE* input, int** program) {
    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int entry[3] = { 0 };
        entry[0] = get_operation(strtok(buffer, " "));

        char* strparam1 = strtok(NULL, ",");
        entry[1] = atoi(strparam1);
        if (entry[1] == 0) entry[1] = strparam1[0] - 97;

        char* strparam2 = strtok(NULL, ",");
        if (strparam2 != NULL) {
            entry[2] = atoi(strparam2);
        }
        da_append(*program, entry);
    }
}

int run_program(int* program, int a_start) {
    int reg[2] = { a_start, 0 };
    ssize_t ip = 0;
    ssize_t n = da_length(program);

    while (ip >= 0 && ip < n) {
        int op = program[ip * 3 + 0];
        int arg1 = program[ip * 3 + 1];
        int arg2 = program[ip * 3 + 2];
        switch (op) {
            case HLF: reg[arg1] /= 2;
                break;
            case TPL: reg[arg1] *= 3;
                break;
            case INC: reg[arg1] += 1;
                break;
            case JMP: ip += arg1;
                continue;
            case JIE: ip += (reg[arg1] % 2 == 0) ? arg2 : 1;
                continue;
            case JIO: ip += (reg[arg1] == 1) ? arg2 : 1;
                continue;
        }
        ++ip;
    }
    return reg['b' - 97];
}

int main() {
    FILE* input = fopen("data/day_23.txt", "r");
    int* program = da_create(3 * sizeof(int));
    parse_input(input, &program);

    printf("Part 1: %d\n", run_program(program, 0));
    printf("Part 2: %d\n", run_program(program, 1));
    
    da_free(program);
    fclose(input);
}