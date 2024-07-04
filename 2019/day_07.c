#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"

int get_output(int* program, int arr[5]) {
    IC_state* amplifiers[5];
    for (int i = 0; i < 5; ++i) {
        amplifiers[i] = ic_create(program);
        ic_push_input(amplifiers[i], arr[i]);
    }
    int signal = 0;
    for (int i = 0; i < 5; ++i) {
        ic_push_input(amplifiers[i], signal);
        ic_interpret(amplifiers[i]);
        signal = ic_pop_output(amplifiers[i]);
    }
    for (int i = 0; i < 5; ++i)
        ic_free(amplifiers[i]);
    return signal;
}

int get_output_feedback(int* program, int arr[5]) {
    IC_state* amplifiers[5];
    for (int i = 0; i < 5; ++i) {
        amplifiers[i] = ic_create(program);
        ic_push_input(amplifiers[i], arr[i] + 5);
    }
    int signal = 0;
    bool halt = false;
    while (!halt) {
        IC_stop status;
        for (int i = 0; i < 5; ++i) {
            ic_push_input(amplifiers[i], signal);
            status = ic_interpret(amplifiers[i]);
            signal = ic_pop_output(amplifiers[i]);
        }
        if (status == IC_HALT)
            halt = true;
    }
    for (int i = 0; i < 5; ++i)
        ic_free(amplifiers[i]);
    return signal;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void permute(int* program, int arr[5], int start, int end, int* pbest_output, int* pbest_output_feedback) {
    if (start == end) {
        int output = get_output(program, arr);
        if (output > *pbest_output)
            *pbest_output = output;
        int output_feedback = get_output_feedback(program, arr);
        if (output_feedback > *pbest_output_feedback)
            *pbest_output_feedback = output_feedback;
    } else {
        for (int i = start; i <= end; i++) {
            swap(&arr[start], &arr[i]);
            permute(program, arr, start + 1, end, pbest_output, pbest_output_feedback);
            swap(&arr[start], &arr[i]);
        }
    }
}

void test_permutations(int* program, int arr[5], int* pbest_output, int* pbest_output_feedback) {
    permute(program, arr, 0, 4, pbest_output, pbest_output_feedback);
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");
    int* program = ic_parse(input);
    fclose(input);

    int arr[5] = { 0, 1, 2, 3, 4 };
    int best_output = 0;
    int best_output_feedback = 0;
    test_permutations(program, arr, &best_output, &best_output_feedback);

    printf("Part 1: %d\n", best_output);
    printf("Part 2: %d\n", best_output_feedback);
    da_free(program);
}