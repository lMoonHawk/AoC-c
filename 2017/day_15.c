#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define LINE_MAX 256
#define GEN_A_FACT 16807
#define GEN_B_FACT 48271
#define GEN_A_MUL 4
#define GEN_B_MUL 8
#define GEN_MOD 2147483647

uint32_t parse_line_gen(FILE* input, char buffer[LINE_MAX]) {
    fgets(buffer, LINE_MAX, input);
    strtok(buffer, " ");
    for (int i = 0; i < 3; ++i) strtok(NULL, " ");
    return atoi(strtok(NULL, " "));
}

uint32_t gen_next(uint32_t* value, int factor, int mul) {
    do {
        *value = ((uint64_t)*value * factor) % GEN_MOD;
    } while (*value % mul != 0);
    return *value;
}

static inline bool is_matching(uint32_t v1, uint32_t v2) {
    return (v1 & 0xFFFF) == (v2 & 0xFFFF);
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    uint32_t gen_a_p1 = parse_line_gen(input, buffer);
    uint32_t gen_b_p1 = parse_line_gen(input, buffer);
    fclose(input);
    uint32_t gen_a_p2 = gen_a_p1;
    uint32_t gen_b_p2 = gen_b_p1;
    int total1 = 0, total2 = 0;
    for (int i = 0; i < 40000000; ++i)
        total1 += is_matching(gen_next(&gen_a_p1, GEN_A_FACT, 1), gen_next(&gen_b_p1, GEN_B_FACT, 1));
    for (int i = 0; i < 5000000; ++i)
        total2 += is_matching(gen_next(&gen_a_p2, GEN_A_FACT, GEN_A_MUL), gen_next(&gen_b_p2, GEN_B_FACT, GEN_B_MUL));

    printf("Part 1: %d\n", total1);
    printf("Part 2: %d\n", total2);
}