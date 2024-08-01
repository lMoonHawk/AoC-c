#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    ADD,
    MUL,
    OP_COUNT,
} Op;

bool next_expression(FILE* input, char expression[LINE_MAX]) {
    int n = 0;
    int c = fgetc(input);
    while (c != '\n' && c != EOF) {
        if (c != ' ' && c != '\r')
            expression[n++] = c;
        c = fgetc(input);
    }
    expression[n] = '\0';
    return n > 0;
}

Op get_op(char op) {
    if (op == '+')
        return ADD;
    else if (op == '*')
        return MUL;
    return OP_COUNT;
}

void eval_stack_push(char** op_stack, int64_t** out) {
    char operation;
    int64_t arg1, arg2;
    da_pop(*op_stack, &operation);
    da_pop(*out, &arg1);
    da_pop(*out, &arg2);
    if (operation == '+')
        da_append(*out, (int64_t) { arg1 + arg2 });
    else if (operation == '*')
        da_append(*out, (int64_t) { arg1* arg2 });

}

int64_t eval(char expression[LINE_MAX], int prec[OP_COUNT]) {
    // Shunting-yard algorithm with eval
    int64_t* out = da_create(sizeof(*out));
    char* op_stack = da_create(1);

    for (char* token = expression; *token != '\0'; ++token) {
        if (*token >= '0' && *token <= '9') {
            da_append(out, (int64_t) { *token - '0' });
        } else {
            while (da_length(op_stack) && op_stack[da_length(op_stack) - 1] != '(' && *token != '(') {
                if (!((*token != '+' && *token != '*') || prec[get_op(op_stack[da_length(op_stack) - 1])] >= prec[get_op(*token)]))
                    break;
                eval_stack_push(&op_stack, &out);
            }
            if (*token == ')') {
                da_pop(op_stack, NULL);
            } else {
                da_append(op_stack, *token);
            }
        }
    }
    while (da_length(op_stack)) {
        eval_stack_push(&op_stack, &out);
    }
    int64_t result = out[0];
    da_free(op_stack);
    da_free(out);
    return result;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");

    int precedance_p1[OP_COUNT] = {
        [ADD] = 0,
        [MUL] = 0,
    };
    int precedance_p2[OP_COUNT] = {
        [ADD] = 1,
        [MUL] = 0
    };
    
    int64_t result_p1 = 0;
    int64_t result_p2 = 0;
    char expression[LINE_MAX];
    while (next_expression(input, expression)) {
        result_p1 += eval(expression, precedance_p1);
        result_p2 += eval(expression, precedance_p2);
    }
    fclose(input);

    printf("Part 1: %" PRId64 "\n", result_p1);
    printf("Part 2: %" PRId64 "\n", result_p2);
}