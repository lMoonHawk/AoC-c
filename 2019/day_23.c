#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

int main() {
    FILE* input = fopen("data/day_23.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    IC_state** states = malloc(sizeof(IC_state*) * 50);
    for (int i = 0; i < 50; ++i) {
        states[i] = ic_create(program);
        ic_input_push(states[i], i);
    }
    da_free(program);

    int64_t nat_x = 0;
    int64_t nat_y = 0;
    int64_t first_nat_y = 0;
    int64_t last_nat_y = 0;
    while (1) {
        bool idle = true;
        for (int i = 0; i < 50; ++i) {
            if (da_length(states[i]->input->arr) == 0) {
                ic_input_push(states[i], -1);
            } else {
                idle = false;
            }
            ic_interpret(states[i]);
            while (ic_output_length(states[i])) {
                int64_t addr = ic_output_pop(states[i]);
                int64_t x = ic_output_pop(states[i]);
                int64_t y = ic_output_pop(states[i]);
                if (addr == 255) {
                    nat_x = x;
                    nat_y = y;
                    if (first_nat_y == 0)
                        first_nat_y = y;
                } else {
                    ic_input_push(states[addr], x);
                    ic_input_push(states[addr], y);
                }
            }
        }
        if (idle && nat_x != 0 && nat_y != 0) {
            if (nat_y == last_nat_y)
                break;
            ic_input_push(states[0], nat_x);
            ic_input_push(states[0], nat_y);
            last_nat_y = nat_y;
        }
    }
    
    for (int i = 0; i < 50; ++i)
        ic_free(states[i]);
    free(states);

    printf("Part 1: %ld\n", first_nat_y);
    printf("Part 2: %ld\n", nat_y);
}
