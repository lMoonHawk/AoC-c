#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define LINE_MAX 256
#define MOD 20201227

int main() {
    FILE* input = fopen("data/day_25.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, LINE_MAX, input);
    uint64_t card_pk = atoi(buffer);
    fgets(buffer, LINE_MAX, input);
    uint64_t door_pk = atoi(buffer);
    fclose(input);

    int card_ls = 0;
    uint64_t value = 1;
    while (value != card_pk) {
        ++card_ls;
        value = (value * 7) % MOD;
    }

    value = 1;
    for (int i = 0; i < card_ls; ++i) {
        value = (value * door_pk) % MOD;
    }

    printf("Part 1: %" PRIu64 "\n", value);
    printf("Part 2: \n");
}