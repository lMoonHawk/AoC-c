#ifndef INTCODE_H
#define INTCODE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    ADD = 1,
    MUL = 2,
    INPUT = 3,
    OUTPUT = 4,
    JNZ = 5,
    JZ = 6,
    LT = 7,
    EQ = 8,
    BASE_ADD = 9,
    HALT = 99,
} Opcode;

typedef enum {
    IC_HALT,
    IC_EMPTY_INPUT,
    IC_SINGLE_OUTPUT,
} IC_stop;

typedef enum {
    IC_POSITION,
    IC_IMMEDIATE,
    IC_RELATIVE,
} IC_mode;

typedef struct {
    int64_t* program;
    int64_t base;
    size_t ip;
    Queue input;
    Queue output;
} IC_state;

int64_t* ic_parse(FILE* input);
IC_state* ic_create(int64_t* program);
void ic_free(IC_state* state);
void ic_push_input(IC_state* state, int64_t value);
int64_t ic_pop_output(IC_state* state);
void get_parameters(int64_t parameters[3], int count, int modes, bool writes, IC_state* state);
void _ic_write(IC_state* state, size_t addr, int64_t value);
int64_t _ic_get(IC_state* state, size_t addr);
IC_stop ic_interpret(IC_state* state);

#endif // INTCODE_H

#ifdef INTCODE_IMPLEMENTATION

int64_t* ic_parse(FILE* input) {
    int64_t* program = da_create(sizeof(*program));
    char c = fgetc(input);
    int64_t buffer = 0;
    int sign = 1;
    while (c != '\n' && c != '\r' && c != EOF) {
        if (c == ',') {
            da_append(program, (int64_t) { sign* buffer });
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
    da_append(program, (int64_t) { sign* buffer });
    return program;
}

IC_state* ic_create(int64_t* program) {
    IC_state* state = malloc(sizeof(*state));
    state->program = da_copy(program);
    state->ip = 0;
    state->base = 0;
    state->input = q_create(sizeof(int64_t));
    state->output = q_create(sizeof(int64_t));
    return state;
}

void ic_free(IC_state* state) {
    da_free(state->program);
    q_free(state->input);
    q_free(state->output);
    free(state);
}

void ic_push_input(IC_state* state, int64_t value) {
    q_push(state->input, value);
}

int64_t ic_pop_output(IC_state* state) {
    int64_t output;
    q_pop(state->output, &output);
    return output;
}

void _ic_write(IC_state* state, size_t addr, int64_t value) {
    if (addr >= da_length(state->program)) {
        for (size_t i = da_length(state->program); i <= addr; ++i) {
            da_append(state->program, (int64_t) { 0 });
        }
    }
    state->program[addr] = value;
}

int64_t _ic_get(IC_state* state, size_t addr) {
    if (addr >= da_length(state->program))
        return 0;
    return state->program[addr];
}

void get_parameters(int64_t parameters[3], int count, int modes, bool writes, IC_state* state) {
    for (int i = 0; i < count; ++i) {
        IC_mode mode = modes % 10;
        int64_t parameter = _ic_get(state, state->ip + i + 1);
        bool is_write_addr = (i == (count - 1)) && writes;
        if (mode == IC_RELATIVE)
            parameter = parameter + state->base;
        if (is_write_addr || (mode == IC_IMMEDIATE))
            parameters[i] = parameter;
        else
            parameters[i] = _ic_get(state, parameter);
        modes /= 10;
    }
}

IC_stop ic_interpret(IC_state* state) {
    while (1) {
        int packed_code = state->program[state->ip];
        Opcode opcode = packed_code % 100;
        int modes = packed_code / 100;
        int64_t parameters[3] = { 0 };

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
                state->ip = (parameters[0] != 0) ? (size_t)parameters[1] : state->ip + 3;
                break;
            case JZ:
                get_parameters(parameters, 2, modes, false, state);
                state->ip = (parameters[0] == 0) ? (size_t)parameters[1] : state->ip + 3;
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
            case BASE_ADD:
                get_parameters(parameters, 1, modes, false, state);
                state->base += parameters[0];
                state->ip += 2;
                break;
            case HALT:
                return IC_HALT;
        }
    }
}

#endif // INTCODE_IMPLEMENTATION