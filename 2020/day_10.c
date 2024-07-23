#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int* parse_input(FILE* input) {
    int* jolts = da_create(sizeof(*jolts));
    char buffer[LINE_MAX] = { 0 };
    int number = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%d", &number);
        da_append(jolts, number);
    }
    return jolts;
}

int int_cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");
    int* jolts = parse_input(input);
    fclose(input);
    int n = da_length(jolts);
    qsort(jolts, n, sizeof(*jolts), int_cmp);

    int diff[3] = { 0, 0, 1 };
    int current_adapter = 0;
    for (int i = 0; i < n; ++i) {
        ++diff[jolts[i] - current_adapter - 1];
        current_adapter = jolts[i];
    }

    int64_t* ways_tab = calloc(n, sizeof(*ways_tab));
    for (int i = 0; i < n; ++i) {
        int64_t ways = 0;
        for (int j = i - 1; j >= 0 && j >= i - 3; --j)
            ways += (jolts[i] - jolts[j] <= 3) * ways_tab[j];
        if (i < 3)
            ways++;
        ways_tab[i] = ways;
    }
    int64_t total_ways = ways_tab[n - 1];
    da_free(jolts);
    free(ways_tab);

    printf("Part 1: %d\n", diff[0] * diff[2]);
    printf("Part 2: %" PRId64 "\n", total_ways);
}