#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LINE_MAX 256

long part1(long num) {
    long dnum = num / 10;
    for (long k = 2; ; k += 2) {
        long sum = 0;
        for (long i = 1; i < pow(k, .5) + 1; ++i) if (k % i == 0) sum += i + k / i;
        if (sum >= dnum) return k;
    }
}

long part2(long num) {
    div_t nd = div(num, 11);
    nd.quot += nd.rem > 0;
    for (long k = 2; ; k += 2) {
        long sum = 0;
        for (long i = 1; i <= 50; ++i) if (k % i == 0) sum += k / i;
        if (sum >= nd.quot) return k;
    }
}

int main() {
    FILE* input = fopen("data/day_20.txt", "r");
    char buffer[LINE_MAX];

    fgets(buffer, sizeof(buffer), input);
    long num = atol(buffer);

    printf("Part 1: %ld\n", part1(num));
    printf("Part 2: %ld", part2(num));
}