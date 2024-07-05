#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

int main() {
    FILE* input = fopen("data/day_09.txt", "r");
    int64_t *program = ic_parse(input);
    fclose(input);

    IC_state* state_test = ic_create(program);
    ic_push_input(state_test, 1);
    ic_interpret(state_test);
    int64_t output_test = ic_pop_output(state_test);
    ic_free(state_test);

    IC_state* state_sensor = ic_create(program);
    ic_push_input(state_sensor, 2);
    ic_interpret(state_sensor);
    int64_t output_coordinates = ic_pop_output(state_sensor);
    ic_free(state_sensor);
    
    da_free(program);

    printf("Part 1: %ld\n", output_test);
    printf("Part 2: %ld\n", output_coordinates);
}