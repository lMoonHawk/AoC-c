#ifndef INTCODE_H
#define INTCODE_H

#include <stdio.h>

typedef enum {
    ADD = 1,
    MUL = 2,
    INPUT = 3,
    OUTPUT = 4,
    JNZ = 5,
    JZ = 6,
    LT = 7,
    EQ = 8,
    HALT = 99,
} Opcode;

typedef enum {
    IC_HALT,
    IC_EMPTY_INPUT,
} IC_stop;

typedef struct {
    int* program;
    int ip;
    Queue input;
    Queue output;
} IC_state;

int* ic_parse(FILE* input);
IC_state* ic_create(int* program);
void ic_free(IC_state* state);
void ic_push_input(IC_state* state, int value);

#endif // INTCODE_H

#ifdef INTCODE_IMPLEMENTATION

int* ic_parse(FILE* input) {
    int* program = da_create(sizeof(*program));
    char c = fgetc(input);
    int buffer = 0;
    int sign = 1;
    while (c != '\n' && c != '\r' && c != EOF) {
        if (c == ',') {
            da_append(program, (int) { sign * buffer });
            buffer = 0;
            sign = 1;
        } else {
            if (buffer == 0 && c == '-')
                sign = -1;
            else
                buffer = buffer * 10 + c - '0';
        }
        c = fgetc(input);
    }
    da_append(program, buffer);
    return program;
}

IC_state* ic_create(int* program) {
    IC_state* state = malloc(sizeof(*state));
    state->program = da_copy(program);
    state->ip = 0;
    state->input = q_create(sizeof(int));
    state->output = q_create(sizeof(int));
    return state;
}

void ic_free(IC_state* state) {
    da_free(state->program);
    q_free(state->input);
    q_free(state->output);
    free(state);
}

void ic_push_input(IC_state* state, int value) {
    q_push(state->input, value);
}

#endif // INTCODE_IMPLEMENTATION