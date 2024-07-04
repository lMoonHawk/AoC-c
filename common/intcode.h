#ifndef INTCODE_H
#define INTCODE_H

#include <stdio.h>
#include <stdbool.h>

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
    IC_SINGLE_OUTPUT,
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
int ic_pop_output(IC_state* state);
void get_parameters(int parameters[3], int count, int modes, bool writes, IC_state* state);
void _ic_write(IC_state* state, size_t addr, int value);
IC_stop ic_interpret(IC_state* state);

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

int ic_pop_output(IC_state* state) {
    int output;
    q_pop(state->output, &output);
    return output;
}

void get_parameters(int parameters[3], int count, int modes, bool writes, IC_state* state) {
    for (int i = 0; i < count; ++i) {
        int parameter = state->program[state->ip + i + 1];
        if ((modes % 10) || ((i == (count - 1)) && writes)) {
            parameters[i] = parameter;
        } else {
            parameters[i] = state->program[parameter];
        }
        modes /= 10;
    }
}

void _ic_write(IC_state* state, size_t addr, int value) {
    if (addr >= da_length(state->program))
        _da_resize((void**)&state->program, addr + 1, sizeof(int));
    state->program[addr] = value;
}

IC_stop ic_interpret(IC_state* state) {
    while (1) {
        int packed_code = state->program[state->ip];
        Opcode opcode = packed_code % 100;
        int modes = packed_code / 100;
        int parameters[3] = { 0 };

        switch (opcode) {
            case ADD:
                get_parameters(parameters, 3, modes, true, state);
                _ic_write(state, parameters[2], parameters[0] + parameters[1]);
                state->ip += 4;
                break;
            case MUL:
                get_parameters(parameters, 3, modes, true, state);
                _ic_write(state, parameters[2], parameters[0] * parameters[1]);
                state->ip += 4;
                break;
            case INPUT:
                get_parameters(parameters, 1, modes, true, state);
                if (q_length(state->input))
                    q_pop(state->input, &state->program[parameters[0]]);
                else
                    return IC_EMPTY_INPUT;
                state->ip += 2;
                break;
            case OUTPUT:
                get_parameters(parameters, 1, modes, false, state);
                q_push(state->output, parameters[0]);
                state->ip += 2;
                break;
            case JNZ:
                get_parameters(parameters, 2, modes, false, state);
                state->ip = (parameters[0] != 0) ? parameters[1] : state->ip + 3;
                break;
            case JZ:
                get_parameters(parameters, 2, modes, false, state);
                state->ip = (parameters[0] == 0) ? parameters[1] : state->ip + 3;
                break;
            case LT:
                get_parameters(parameters, 3, modes, true, state);
                _ic_write(state, parameters[2], parameters[0] < parameters[1]);
                state->ip += 4;
                break;
            case EQ:
                get_parameters(parameters, 3, modes, true, state);
                _ic_write(state, parameters[2], parameters[0] == parameters[1]);
                state->ip += 4;
                break;
            case HALT:
                return IC_HALT;
        }
    }
}

#endif // INTCODE_IMPLEMENTATION