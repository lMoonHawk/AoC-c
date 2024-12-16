#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define _LINE_MAX 256

typedef struct {
    Point2 a, b, prize;
} Machine;

bool parse_machine(FILE* input, Machine* machine) {
    char buffer[_LINE_MAX] = { 0 };
    if (fgets(buffer, sizeof(buffer), input) == NULL)
        return false;
    sscanf(buffer, "Button A: X%d, Y%d", &machine->a.x, &machine->a.y);
    fgets(buffer, sizeof(buffer), input);
    sscanf(buffer, "Button B: X%d, Y%d", &machine->b.x, &machine->b.y);
    fgets(buffer, sizeof(buffer), input);
    sscanf(buffer, "Prize: X=%d, Y=%d", &machine->prize.x, &machine->prize.y);
    fgets(buffer, sizeof(buffer), input);
    return true;
}

int64_t min_tokens(Machine machine, int64_t offset) {
    int64_t num_b = (offset + machine.prize.x) * machine.a.y - machine.a.x * (offset + machine.prize.y);
    int64_t den_b = machine.a.y * machine.b.x - machine.a.x * machine.b.y;
    int64_t b_quot = num_b / den_b;
    int64_t b_rem = num_b % den_b;

    int64_t num_a = (offset + machine.prize.x) - b_quot * machine.b.x;
    int64_t a_quot = num_a / machine.a.x;
    int64_t a_rem = num_a % machine.a.x;

    return (a_rem == 0 && b_rem == 0) * (a_quot * 3 + b_quot);
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");
    Machine machine;
    int64_t tokens = 0;
    int64_t tokens_offset = 0;
    while (parse_machine(input, &machine)) {
        tokens += min_tokens(machine, 0);
        tokens_offset += min_tokens(machine, 10000000000000);
    }
    fclose(input);
    
    printf("Part 1: %" PRId64 " \n", tokens);
    printf("Part 2: %" PRId64 " \n", tokens_offset);
}