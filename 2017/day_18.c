#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"

#define LINE_MAX 256
#define LETTERS 26

int64_t mod(int64_t n, int64_t m) {
    return ((n % m) + m) % m;
}

typedef enum {
    SND, SET, ADD, MUL, MOD, RCV, JGZ,
    OP_CNT,
} Op;

typedef struct {
    Op op;
    int64_t args[2];
    bool literal[2];
} Instruction;

typedef struct {
    int ip;
    int64_t registers[LETTERS];
    Queue q;
} Program;

Program* program_create(int pid) {
    Program* p = calloc(1, sizeof(*p));
    p->registers['p' - 'a'] = pid;
    p->q = q_create(sizeof(int64_t));
    return p;
}

void program_free(Program* p) {
    q_free(p->q);
    free(p);
}

int64_t eval(Instruction ins, int64_t* reg, int arg_i) {
    return ins.literal[arg_i] ? ins.args[arg_i] : reg[ins.args[arg_i]];
}

void parse_op(const char* op_str, Instruction* ins) {
    char* ops[] = { "snd", "set", "add", "mul", "mod", "rcv", "jgz" };
    for (Op i = 0; i < OP_CNT; ++i) {
        if (strcmp(op_str, ops[i]) == 0) {
            ins->op = i;
            return;
        }
    }
    assert(0 && "Unreachable");
}

void parse_arg(const char* arg, Instruction* ins, int arg_i) {
    if (isdigit(arg[0]) || arg[0] == '-') {
        ins->args[arg_i] = atoi(arg);
        ins->literal[arg_i] = true;
    } else {
        ins->args[arg_i] = arg[0] - 'a';
        ins->literal[arg_i] = false;
    }
}

Instruction* parse_input(FILE* input) {
    Instruction* instructions = da_create(sizeof(*instructions));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Instruction instruction = { 0 };
        parse_op(strtok(buffer, " "), &instruction);
        parse_arg(strtok(NULL, " "), &instruction, 0);
        if (instruction.op != SND && instruction.op != RCV)
            parse_arg(strtok(NULL, " "), &instruction, 1);
        da_append(instructions, instruction);
    }
    return instructions;
}

void run_until_wait(Program* p, Instruction* instructions, Queue send) {
    int n = da_length(instructions);
    while (p->ip >= 0 && p->ip < n) {
        Instruction ins = instructions[p->ip];
        switch (ins.op) {
            case SND:
                int64_t val = eval(ins, p->registers, 0);
                q_push(send, val);
                break;
            case SET:
                p->registers[ins.args[0]] = eval(ins, p->registers, 1);
                break;
            case ADD:
                p->registers[ins.args[0]] += eval(ins, p->registers, 1);
                break;
            case MUL:
                p->registers[ins.args[0]] *= eval(ins, p->registers, 1);
                break;
            case MOD:
                p->registers[ins.args[0]] = mod(p->registers[ins.args[0]], eval(ins, p->registers, 1));
                break;
            case RCV:
                if (p->q == NULL || !q_length(p->q))
                    return;
                q_pop(p->q, &(p->registers[ins.args[0]]));
                break;
            case JGZ:
                if (eval(ins, p->registers, 0) > 0) {
                    p->ip += eval(ins, p->registers, 1) - 1;
                }
                break;
            case OP_CNT:
                break;
        }
        ++p->ip;
    }
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");
    Instruction* instructions = parse_input(input);
    fclose(input);

    Program* p = program_create(0);
    Queue sent = q_create(sizeof(int64_t));
    run_until_wait(p, instructions, sent);
    int64_t val;
    while (q_length(sent))
        q_pop(sent, &val);
    program_free(p);
    q_free(sent);

    Program* p0 = program_create(0);
    Program* p1 = program_create(1);
    int cnt_p1_snt = 0;
    do {
        cnt_p1_snt += q_length(p0->q);
        run_until_wait(p0, instructions, p1->q);
        run_until_wait(p1, instructions, p0->q);
    } while(q_length(p0->q) || q_length(p1->q));

    program_free(p0);
    program_free(p1);
    da_free(instructions);
 
    printf("Part 1: %lu\n", val);
    printf("Part 2: %d\n", cnt_p1_snt);
}