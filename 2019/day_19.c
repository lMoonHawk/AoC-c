#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

bool is_affected(int64_t* program, int i, int j) {
    IC_state* state = ic_create(program);
    ic_input_push(state, i);
    ic_input_push(state, j);
    ic_interpret(state);
    bool result = ic_output_pop(state);
    ic_free(state);
    return result;
}

int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    int affected_points = 0;
    for (int i = 0; i < 50; ++i)
        for (int j = 0; j < 50; ++j)
            affected_points += is_affected(program, i, j);


    int x = 0, y = 99;
    while (true) {
        if (is_affected(program, x, y)) {
            if (is_affected(program, x + 99, y - 99))
                break;
            ++y;
        } else {
            ++x;
        }
    }
    da_free(program);
        
    printf("Part 1: %d\n", affected_points);
    printf("Part 2: %d\n", x * 10000 + y - 99);
}