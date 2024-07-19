#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

void ic_input_push_string(IC_state* state, char* str, bool newline) {
    while (*str != '\0')
        ic_input_push(state, *(str++));
    if (newline)
        ic_input_push(state, '\n');
}

void ic_output_print(IC_state* state) {
    while (ic_output_length(state))
        printf("%c", (char)ic_output_pop(state));
}

int main() {
    /* Items: 
        mutex
        space law space brochure
        hologramn
        manifold
    */
    FILE* input = fopen("data/day_25.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);
    IC_state* state = ic_create(program);

    IC_stop stop = ic_interpret(state);
    while (stop != IC_HALT) {
        ic_output_print(state);
        char s[16];
        printf(">> ");
        fgets(s, 16, stdin);
        ic_input_push_string(state, s, false);
        stop = ic_interpret(state);
    }
    ic_output_print(state);
    ic_free(state);
    da_free(program);
}