#include <stdio.h>
#include <stdlib.h>

#define LINE_MAX 256

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

typedef struct {
    Point2 pos;
    int value;
} Mem;

int sum_adj(Point2 pos, Mem* mem) {
    size_t n = da_length(mem);
    int out = 0;
    int cnt = 0;
    for (size_t i = n - 1; i < n; --i) {
        for (int mx = pos.x - 1; mx <= pos.x + 1; ++mx) {
            for (int my = pos.y - 1; my <= pos.y + 1; ++my) {
                Point2 neigh = { mx, my };
                if (neigh.x == mem[i].pos.x && neigh.y == mem[i].pos.y) {
                    out += mem[i].value;
                    ++cnt;
                }
                if (cnt == 4)
                    return out;
            }
        }
    }
    return out;
}

void traverse(int num, int* steps, int* overflow) {
    Point2 moves[] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

    int k = 0, inc = 0;
    Mem* mem = da_create(sizeof(*mem));

    int value = 1;
    *steps = 0;
    *overflow = 0;

    Point2 pos = { 0 };
    da_append(mem, ((Mem){pos, 1}));
    while (1) {
        Point2 move = moves[k % 4];
        if ((move.x == 1 && move.y == 0) || (move.x == -1 && move.y == 0))
            inc += 1;
        for (int i = 0; i < inc; ++i) {
            pos = p2add(pos, move);

            if (++value == num) {
                *steps = p2dist(pos, (Point2) { 0 });
            }
            if (*overflow == 0) {
                int sum = sum_adj(pos, mem);
                if (sum > num)
                    *overflow = sum;
                da_append(mem, ((Mem){pos, sum}));
            }
            if (*steps > 0 && *overflow > 0) {
                da_free(mem);
                return;
            }
        }
        ++k;
    }
}

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);

    int num = atoi(buffer);
    int steps, overflow;
    traverse(num, &steps, &overflow);

    printf("Part 1: %d\n", steps);
    printf("Part 2: %d\n", overflow);
}
