#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

void ic_input_push_string(IC_state* state, char* str) {
    while (*str != '\0')
        ic_input_push(state, *(str++));
    ic_input_push(state, '\n');
}

int main() {
    FILE* input = fopen("data/day_21.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    IC_state* state_p1 = ic_create(program);

    char* instuctions_p1[] = {
        "NOT A T",
        "NOT B J",
        "OR T J",
        "NOT C T",
        "OR T J",
        "AND D J",
        "WALK"
    };
    for (size_t i = 0; i < sizeof(instuctions_p1) / sizeof(char*); ++i)
        ic_input_push_string(state_p1, instuctions_p1[i]);

    ic_interpret(state_p1);
    int p1 = 0;
    while (ic_output_length(state_p1))
        p1 = ic_output_pop(state_p1);
    ic_free(state_p1);

    IC_state* state_p2 = ic_create(program);
    char* instuctions_p2[] = {
        "NOT A T",
        "NOT B J",
        "OR T J",
        "NOT C T",
        "OR T J",
        "AND D J",
        "AND E T",
        "OR H T",
        "AND T J",
        "RUN"
    };
    for (size_t i = 0; i < sizeof(instuctions_p2) / sizeof(char*); ++i)
        ic_input_push_string(state_p2, instuctions_p2[i]);
    ic_interpret(state_p2);
    int p2 = 0;
    while (ic_output_length(state_p2))
        p2 = ic_output_pop(state_p2);
    ic_free(state_p2);

    da_free(program);
 
    printf("Part 1: %d\n", p1);
    printf("Part 2: %d\n", p2);
}