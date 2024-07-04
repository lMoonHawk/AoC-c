#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

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