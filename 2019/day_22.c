#include <stdio.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define int128_t __int128_t

typedef enum {
    STACK,
    CUT,
    INCREMENT,
} Technique;

typedef struct {
    Technique type;
    int amount;
} Shuffle;

int64_t mod(int64_t a, int64_t b) {
    return (a % b + b) % b;
}

int64_t gcdExtended(int64_t a, int64_t b, int64_t* x, int64_t* y) {
    if (a == 0) {
        *x = 0, * y = 1;
        return b;
    }
    int64_t x1, y1;
    int64_t gcd = gcdExtended(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}

int64_t modinv(int64_t a, int64_t m) {
    int64_t x, y;
    int64_t res;
    int64_t g = gcdExtended(a, m, &x, &y);
    if (g != 1) {
        printf("Inverse doesn't exist\n");
        res = 0;
    } else {
        res = (x % m + m) % m;
    }
    return res;
}


int64_t modexp(int64_t base, int64_t exp, int64_t m) {
    int negative_output = 0;
    if (m < 0) {
        negative_output = 1;
        m = -m;
    }
    if (m == 1)
        return 0;
    if (exp < 0) {
        exp = -exp;
        base = modinv(base, m);
    }
    if (base < 0 || base > m) {
        base = mod(base, m);
    }
    int128_t result = 1;
    int128_t base_128 = base;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = result * base_128;
            result %= m;
        }
        exp >>= 1;
        base_128 *= base_128;
        base_128 %= m;
    }
    if (negative_output)
        result = result - m;
    return result;
}


Technique identify_technique(const char* line, int* argument) {
    if (strstr(line, "deal with increment") == line) {
        sscanf(line, "deal with increment %d", argument);
        return INCREMENT;;
    } else if (strstr(line, "cut") == line) {
        sscanf(line, "cut %d", argument);
        return CUT;
    }
    return STACK;
}

Shuffle* parse_input(FILE* input) {
    Shuffle* shuffles = da_create(sizeof(*shuffles));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Shuffle shuffle = { 0 };
        shuffle.type = identify_technique(buffer, &shuffle.amount);
        da_append(shuffles, shuffle);
    }
    return shuffles;
}

int64_t reverse_shuffle(int64_t index, int64_t n, Shuffle* shuffles) {
    for (int i = (int)da_length(shuffles) - 1; i >= 0; --i) {
        switch (shuffles[i].type) {
            case INCREMENT:
                int128_t a = modinv(shuffles[i].amount, n);
                a *= (int128_t)index;
                index = a % n;
                break;
            case CUT:
                index = mod(index + n + shuffles[i].amount, n);
                break;
            case STACK:
                index = n - index - 1;
                break;
        }
    }
    return index;
}

int main() {
    FILE* input = fopen("data/day_22.txt", "r");
    Shuffle* shuffles = parse_input(input);
    fclose(input);

    int index = 2019;
    int n1 = 10007;
    for (size_t i = 0; i < da_length(shuffles); ++i) {
        switch (shuffles[i].type) {
            case INCREMENT:
                index = mod(index * shuffles[i].amount, n1);
                break;
            case CUT:
                index = mod(index - shuffles[i].amount, n1);
                break;
            case STACK:
                index = n1 - index - 1;
                break;
        }
    }
    // reverse_shuffle(index) = a*index + b

    int64_t k = 101741582076661;
    int64_t n = 119315717514047;
    int64_t x = 2020;
    // y = a*x+b
    int64_t y = reverse_shuffle(x, n, shuffles);
    // z = a*y+b
    int64_t z = reverse_shuffle(y, n, shuffles);
    da_free(shuffles);
    //    y - z = a*(x - y)
    // => a = (y-z)/(x-y)
    int128_t a = (int128_t)(y - z) * (int128_t)modinv(x - y + n, n);
    a %= n;
    a += n;
    // => b = y-a*x
    int64_t b = mod((y - a * x), n);
    // f^k(x) = a^k*x + a^(k-1)*b + a^(k-2)*b + ... + b
    //        = a^k*x + b * [a^(k-1) + a^(k-2) + ... + 1]
    //        = a^k*x + b*(a^k-1)/(a-1)
    int64_t e = modexp(a, k, n);

    int64_t p1 = e * x % n;
    int128_t p2 = (int128_t)b * ((int128_t)e - 1) % n;
    int64_t p3 = modinv(a - 1, n) % n;
    int64_t p4 = p2 * p3 % n;
    int64_t res = (p1 + p4) % n;

    printf("Part 1: %d\n", index);
    printf("Part 2: %ld\n", (long int)res);
}