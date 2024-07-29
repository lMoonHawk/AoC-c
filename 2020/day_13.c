#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int id, minute;
} Bus;

static inline int64_t mod(int64_t a, int64_t b) {
    return (a % b + b) % b;
}

Bus* parse_input(FILE* input, int* earliest) {
    char buffer[LINE_MAX];
    fgets(buffer, LINE_MAX, input);
    *earliest = atoi(buffer);
    fgets(buffer, LINE_MAX, input);

    Bus* buses = da_create(sizeof(*buses));
    int minute = 0;
    char* tok = strtok(buffer, ",");
    while (tok != NULL) {
        if (tok[0] != 'x')
            da_append(buses, ((Bus) { atoi(tok), minute }));
        tok = strtok(NULL, ",");
        ++minute;
    }
    return buses;
}

int64_t gcd_ext(int64_t a, int64_t b, int64_t* x, int64_t* y) {
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
    int64_t x1, y1;
    int64_t g = gcd_ext(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
    return g;
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");
    int earliest = 0;
    Bus* buses = parse_input(input, &earliest);
    fclose(input);
    int time = earliest - 1;
    int id = -1;
    while (id == -1) {
        ++time;
        for (size_t i = 0; i < da_length(buses); ++i) {
            if (time % buses[i].id == 0) {
                id = buses[i].id;
                break;
            }
        }
    }
    /* 
    If bus x must arrive at time t+k, then we have (t+k) ≡ 0 (mod x) <=> t ≡ x-k (mod x)
    Congruences: buses[i].id - buses[i].minute
    Modulus: buses[i].id
    We search t, shuch that ∀i t ≡ buses[i].id - buses[i].minute (mod buses[i].id)
    */
    int64_t prod = 1;
    for (size_t i = 0; i < da_length(buses); ++i)
        prod *= buses[i].id;

    int64_t t = 0;
    for (size_t i = 0; i < da_length(buses); ++i) {
        /*  
        We use the Chinese Remainder Theorem.
        Multiplying each congruence with n times y (from the Bézout identity a*x + b*y = gcd(a,b))
        Here, gcd(a, b) = 1. The creator of this puzzle is nice and has made it so a and b are always coprime.
        */
        int64_t n = prod / buses[i].id;
        int64_t x, y;
        gcd_ext(buses[i].id, n, &x, &y);
        t += mod((buses[i].id - buses[i].minute) * n * y, prod);
        t %= prod;
    }
    da_free(buses);
    printf("%d\n", (time - earliest) * id);
    printf("%ld\n", t);
}