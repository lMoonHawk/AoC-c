#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    char name[16];
    int value;
} Register;

int reg_get(const char* name, Register* registers) {
    for (size_t i = 0; i < da_length(registers); ++i)
        if (strcmp(registers[i].name, name) == 0)
            return registers[i].value;
    return 0;
}

int reg_incr(const char* name, int incr, Register** registers) {
    for (size_t i = 0; i < da_length(*registers); ++i) {
        if (strcmp((*registers)[i].name, name) == 0) {
            (*registers)[i].value += incr;
            return (*registers)[i].value;
        }
    }
    Register new_reg = { 0 };
    strcpy(new_reg.name, name);
    new_reg.value = incr;
    da_append(*registers, new_reg);
    return 0;
}

int evaluate(char* reg, int increment, char* cond_reg, char* cond_op, int cond_val, Register** registers) {
    bool cond = false;
    int cond_reg_val = reg_get(cond_reg, *registers);

    if (strcmp(cond_op, "==") == 0) {
        cond = cond_reg_val == cond_val;
    } else if (strcmp(cond_op, "!=") == 0) {
        cond = cond_reg_val != cond_val;
    } else if (strcmp(cond_op, ">") == 0) {
        cond = cond_reg_val > cond_val;
    } else if (strcmp(cond_op, "<") == 0) {
        cond = cond_reg_val < cond_val;
    } else if (strcmp(cond_op, ">=") == 0) {
        cond = cond_reg_val >= cond_val;
    } else if (strcmp(cond_op, "<=") == 0) {
        cond = cond_reg_val <= cond_val;
    }
    if (cond)
        return reg_incr(reg, increment, registers);
    return 0;
}

int main() {
    FILE* input = fopen("data/day_08.txt", "r");

    char buffer[LINE_MAX] = { 0 };
    int max_reg_ever = 0;
    Register* registers = da_create(sizeof(*registers));

    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* reg = strtok(buffer, " ");

        int sign = 1;
        if (strcmp(strtok(NULL, " "), "dec") == 0)
            sign = -1;

        int increment = sign * atoi(strtok(NULL, " "));
        strtok(NULL, " ");
        char* cond_reg = strtok(NULL, " ");
        char* cond_op = strtok(NULL, " ");
        int cond_val = atoi(strtok(NULL, " "));

        int reg_value = evaluate(reg, increment, cond_reg, cond_op, cond_val, &registers);
        if (reg_value > max_reg_ever) {
            max_reg_ever = reg_value;
        }
    }
    fclose(input);

    int max_reg = registers[0].value;
    for (size_t i = 1; i < da_length(registers); ++i) {
        if (registers[i].value > max_reg)
            max_reg = registers[i].value;
    }
    da_free(registers);

    printf("Part 1: %d\n", max_reg);
    printf("Part 2: %d\n", max_reg_ever);
}