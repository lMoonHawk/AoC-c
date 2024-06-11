#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int8_t state, next, value, write, move;
} State;

State find_state(char state_name, uint8_t value, State* states) {
    for (size_t i = 0; i < da_length(states); ++i)
        if (state_name == states[i].state && value == states[i].value)
            return states[i];
    assert(0 && "unreachable");
}

State* parse_input(FILE* input, int* end_steps, char* state_begin) {
    char buffer[LINE_MAX] = { 0 };

    fgets(buffer, sizeof(buffer), input);
    for (int i = 0; buffer[i] != '.'; ++i)
        *state_begin = buffer[i];
    fgets(buffer, sizeof(buffer), input);
    strtok(buffer, " ");
    for (int i = 0; i < 4; ++i) strtok(NULL, " ");
    *end_steps = atoi(strtok(NULL, " "));
    fgets(buffer, sizeof(buffer), input);

    State* states = da_create(sizeof(*states));
    State current = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        if (strncmp(buffer, "In state", 8) == 0) {
            current.state = buffer[9];
        } else if (strncmp(buffer, "  If the current value", 22) == 0) {
            current.value = buffer[26] - '0';
        } else if (strncmp(buffer, "    - Write", 11) == 0) {
            current.write = buffer[22] - '0';
        } else if (strncmp(buffer, "    - Move", 10) == 0) {
            current.move = buffer[27] == 'l' ? -1 : 1;
        } else if (strncmp(buffer, "    - Continue", 14) == 0) {
            current.next = buffer[26];
            da_append(states, current);
        }
    }
    return states;
}

int main() {
    FILE* input = fopen("data/day_25.txt", "r");
    char state_name;
    int end_steps;
    State* states = parse_input(input, &end_steps, &state_name);
    fclose(input);

    size_t tape_capacity = 256;
    uint8_t* tape = calloc(tape_capacity, sizeof(*tape));

    ssize_t pointer = 0;
    size_t index = 0;
    int ones = 0;
    // tape stored as indexes 0, (-0), 1, -1, 2, -2, 3, -3, ....
    while (end_steps--) {
        State state = find_state(state_name, tape[index], states);
        ones += (tape[index] == 0) + state.write - 1;

        tape[index] = state.write;
        state_name = state.next;
        pointer += state.move;

        index = pointer >= 0 ? 2 * pointer : -2 * pointer + 1;
        if (index >= tape_capacity) {
            size_t new_tape_capacity = tape_capacity * 2;
            tape = realloc(tape, new_tape_capacity);
            assert(tape != NULL);
            memset(tape + tape_capacity, 0, tape_capacity);
            tape_capacity = new_tape_capacity;
        }
    }
    free(tape);
    printf("Part 1: %d\n", ones);
    printf("Part 2:\n");
    da_free(states);
}