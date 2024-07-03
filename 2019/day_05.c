#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

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

int main() {
    FILE* input = fopen("data/day_05.txt", "r");
    int* program = ic_parse(input);
    fclose(input);

    IC_state* state_p1 = ic_create(program);
    ic_push_input(state_p1, 1);
    ic_interpret(state_p1);
    int output_p1;
    while (q_length(state_p1->output))
        q_pop(state_p1->output, &output_p1);
    ic_free(state_p1);

    IC_state* state_p2 = ic_create(program);
    ic_push_input(state_p2, 5);
    ic_interpret(state_p2);
    int output_p2;
    while (q_length(state_p2->output))
        q_pop(state_p2->output, &output_p2);
    ic_free(state_p2);

    da_free(program);

    printf("Part 1: %d\n", output_p1);
    printf("Part 2: %d\n", output_p2);
}