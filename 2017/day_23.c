#include <stdio.h>
#include <stdbool.h>
// #include <math.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    SET,
    SUB,
    MUL,
    JNZ,
} Operation;

typedef struct {
    Operation op;
    int args[2];
    bool isreg[2];
} Instruction;

Operation parse_op(const char* s) {
    char* ops[] = { "set", "sub", "mul", "jnz" };
    for (size_t i = 0; i < sizeof(ops) / sizeof(char*); ++i)
        if (strcmp(ops[i], s) == 0)
            return i;
    assert(0 && "unreachable");
    return 0;
}

// Returns the argument if literal or get register
int eval(Instruction ins, int arg_i, int* reg) {
    return ins.isreg[arg_i] ? reg[ins.args[arg_i]] : ins.args[arg_i];
}

bool is_prime(int n) {
    if (n == 2)
        return true;
    if (n == 1 || n % 2 == 0)
        return false;
    for (int i = 3; (i - 1) * (i - 1) < n; i += 2)
        if (n % i == 0)
            return false;
    return true;
}

Instruction* parse_input(FILE* input) {
    Instruction* instructions = da_create(sizeof(Instruction));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Instruction ins;
        ins.op = parse_op(strtok(buffer, " "));
        for (int i = 0; i < 2; ++i) {
            char* arg0_str = strtok(NULL, " ");
            if (arg0_str[0] == '-' || (arg0_str[0] >= '0' && arg0_str[0] <= '9')) {
                ins.args[i] = atoi(arg0_str);
                ins.isreg[i] = false;
            } else {
                ins.args[i] = arg0_str[0] - 'a';
                ins.isreg[i] = true;
            }
        }
        da_append(instructions, ins);
    }
    return instructions;
}

int main() {
    FILE* input = fopen("data/day_23.txt", "r");
    Instruction* instructions = parse_input(input);
    fclose(input);
    int n = da_length(instructions);
    int ip = 0;
    int registers[8] = { 0 };
    int mulcnt = 0;

    while (ip >= 0 && ip < n) {
        Instruction ins = instructions[ip];
        switch (ins.op) {
            case SET:
                registers[ins.args[0]] = eval(ins, 1, registers);
                break;
            case SUB:
                registers[ins.args[0]] -= eval(ins, 1, registers);
                break;
            case MUL:
                registers[ins.args[0]] *= eval(ins, 1, registers);
                ++mulcnt;
                break;
            case JNZ:
                ip += eval(ins, 0, registers) ? eval(ins, 1, registers) - 1 : 0;
                break;
        }
        ++ip;
    }
    da_free(instructions);

    /* The instructions can be rewritten as:
    h = 0
    b = 65 * 100 + 100_000
    c = b + 17_000
    while True:
        f = 1
        d = 2
        while True:
            e = 2
            while True:
                if b == d * e:
                    f = 0
                e += 1
                if e == b:
                    break
            d += 1
            if d == b:
                break
        if f == 0:
            h += 1
        if c == b:
            break
        b += 17

    When b can be written as the factor of d and e, f is set to 0 which ultimately increments h.
    This process is done for the starting b up to c */
    int b = 65 * 100 + 100000;
    int c = b + 17000;
    int cnt = 0;
    for (int p = b; p < c + 1; p += 17) {
        cnt += !is_prime(p);
    }
    printf("Part 1: %d\n", mulcnt);
    printf("Part 2: %d\n", cnt);
}