#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#define LETTERS 26
#define _LINE_MAX 256

void parse_input(FILE* input, char rules[LETTERS * LETTERS], uint64_t freq_el[LETTERS], uint64_t freq_pairs[LETTERS * LETTERS]) {
    char buffer[_LINE_MAX];
    fgets(buffer, _LINE_MAX, input);
    int prev;
    for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r'; i++) {
        ++freq_el[buffer[i] - 'A'];
        if (i > 0)
            ++freq_pairs[prev * LETTERS + buffer[i] - 'A'];
        prev = buffer[i] - 'A';
    }
    fgets(buffer, _LINE_MAX, input);
    char pair[3];
    char el;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%s -> %c", pair, &el);
        rules[(pair[0] - 'A') * LETTERS + (pair[1] - 'A')] = el - 'A';
    }
}

uint64_t calc_score(uint64_t freq_el[LETTERS]) {
    uint64_t min = UINT64_MAX;
    uint64_t max = 0;
    for (int i = 0; i < LETTERS; ++i) {
        if (freq_el[i] == 0)
            continue;
        if (freq_el[i] < min)
            min = freq_el[i];
        if (freq_el[i] > max)
            max = freq_el[i];
    }
    return max - min;
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");
    uint64_t freq_el[LETTERS] = { 0 };
    uint64_t freq_pairs[LETTERS * LETTERS] = { 0 };
    char rules[LETTERS * LETTERS];
    parse_input(input, rules, freq_el, freq_pairs);
    fclose(input);

    uint64_t score_p1;
    uint64_t next_freq_pairs[LETTERS * LETTERS];
    memcpy(next_freq_pairs, freq_pairs, sizeof(next_freq_pairs));
    for (int i = 0; i < 40; ++i) {
        if (i == 10)
            score_p1 = calc_score(freq_el);
        for (int left = 0; left < LETTERS; ++left) {
            for (int right = 0; right < LETTERS; ++right) {
                uint64_t count = freq_pairs[left * LETTERS + right];
                if (count == 0)
                    continue;
                int insert = rules[left * LETTERS + right];
                freq_el[insert] += count;
                next_freq_pairs[left * LETTERS + right] -= count;
                next_freq_pairs[left * LETTERS + insert] += count;
                next_freq_pairs[insert * LETTERS + right] += count;
            }
        }
        memcpy(freq_pairs, next_freq_pairs, sizeof(freq_pairs));
    }

    printf("Part 1: %" PRIu64 "\n", score_p1);
    printf("Part 2: %" PRIu64 "\n", calc_score(freq_el));
}