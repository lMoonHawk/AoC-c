#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

static inline int mod(int a, int b) {
    return (a % b + b) % b;
}

int* parse_input(FILE* input) {
    int* signal = da_create(sizeof(int));
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r') {
        da_append(signal, (int) { c - '0' });
        c = fgetc(input);
    }
    return signal;
}

void run_fft(int* in_signal, int* out_signal) {
    int n = da_length(in_signal);
    for (int k = 0; k < n; ++k) {
        int a = 0;
        int b = 0;
        for (int i = k; i < n; i += 4 * (k + 1)) {
            for (int j = i; j < i + k + 1; ++j) {
                if (j >= n) break;
                a += in_signal[j];
            }
        }
        for (int i = 3 * (k + 1) - 1; i < n; i += 4 * (k + 1)) {
            for (int j = i; j < i + k + 1; ++j) {
                if (j >= n) break;
                b += in_signal[j];
            }
        }
        out_signal[k] = abs(a - b) % 10;
    }
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    int* signal = parse_input(input);
    fclose(input);

    int* signal_p1 = da_copy(signal);
    int* signal_buffer = da_copy(signal);
    for (int i = 0; i < 100; ++i) {
        run_fft(signal_p1, signal_buffer);
        int* tmp = signal_p1;
        signal_p1 = signal_buffer;
        signal_buffer = tmp;
    }
    char p1[9] = { 0 };
    for (int i = 0; i < 8; ++i)
        p1[i] = (signal_p1[i] + '0');
    da_free(signal_buffer);
    da_free(signal_p1);



    int offset = 0;
    for (int i = 0; i < 7; ++i)
        offset = offset * 10 + signal[i];
    int n = da_length(signal);
    int m = n * 10000 - offset;

    int* signal_p2 = malloc(m * sizeof(int));
    for (int i = 0; i < m; ++i) {
        signal_p2[i] = signal[mod(-m + i, n)];
    }
    da_free(signal);

    for (int i = 0; i < 100; ++i) {
        for (int j = 1; j < m; ++j) {
            signal_p2[m - 1 - j] += signal_p2[m - 1 - j + 1];
            signal_p2[m - 1 - j] %= 10;
        }
    }
    char p2[9] = { 0 };
    for (int i = 0; i < 8; ++i)
        p2[i] = (signal_p2[i] + '0');
    free(signal_p2);

    printf("Part 1: %s\n", p1);
    printf("Part 2: %s\n", p2);
}