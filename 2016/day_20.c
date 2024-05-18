#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    uint32_t lo, hi;
} Range;

Range* parse_input(FILE* input) {
    Range* ranges = da_create(sizeof(Range));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n') {
        Range range = {
            strtoul(strtok(buffer, "-"), NULL, 10),
            strtoul(strtok(NULL, "-"), NULL, 10)
        };
        da_append(ranges, range);
    }
    return ranges;
}

int range_cmp(const void* _r1, const void* _r2) {
    Range* r1 = (Range*)_r1;
    Range* r2 = (Range*)_r2;
    if (r1->lo < r2->lo) return -1;
    if (r1->lo > r2->lo) return 1;
    if (r1->hi < r2->hi) return -1;
    if (r1->hi > r2->hi) return 1;
    return 0;
}

int main() {
    FILE* input = fopen("data/day_20.txt", "r");
    Range* ranges = parse_input(input);
    size_t n = da_length(ranges);
    fclose(input);
    qsort(ranges, n, sizeof(Range), range_cmp);

    uint32_t lowest_ip = 0;
    int cnt = 0;
    uint32_t value = 0;

    for (size_t i = 0; i < n; ++i) {
        uint32_t lo = ranges[i].lo;
        uint32_t hi = ranges[i].hi;

        if (value > hi)
            continue;
        if (value < lo) {
            if (lowest_ip == 0) lowest_ip = value;
            cnt += lo - value;
        }
        if (hi == UINT32_MAX)
            break;
        value = hi + 1;
    }
    da_free(ranges);

    printf("Part 1: %u\n", lowest_ip);
    printf("Part 2: %d\n", cnt);
}