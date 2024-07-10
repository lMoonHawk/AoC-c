#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

int64_t clamp_input(int64_t d) {
    const int64_t t = d < -1 ? -1 : d;
    return t > 1 ? 1 : t;
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    IC_state* state1 = ic_create(program);
    ic_interpret(state1);
    int blocks = 0;
    while (ic_output_length(state1)) {
        ic_output_pop(state1); // x
        ic_output_pop(state1); // y
        blocks += (ic_output_pop(state1) == 2);
    }
    ic_free(state1);

    IC_state* state2 = ic_create(program);
    ic_override(state2, 0, 2);
    int ball_x = 0;
    int paddle_x = 0;
    int score = 0;
    IC_stop stop = IC_RUNNING;
    while (stop != IC_HALT) {
        ic_input_push(state2, clamp_input(ball_x - paddle_x));
        stop = ic_interpret(state2);

        while (ic_output_length(state2)) {
            int x = ic_output_pop(state2);
            int y = ic_output_pop(state2);
            int tile = ic_output_pop(state2);
            if (x == -1 && y == 0)
                score = tile;
            else if (tile == 3)
                paddle_x = x;
            else if (tile == 4)
                ball_x = x;
        }

    }
    ic_free(state2);
    da_free(program);

    printf("Part 1: %d\n", blocks);
    printf("Part 2: %d\n", score);
}