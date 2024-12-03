#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool expect_str(FILE* input, const char* str) {
    char c;
    int i = 0;
    long start = ftell(input);
    while (str[i] != '\0') {
        c = fgetc(input);
        if (c != str[i]) {
            fseek(input, start, SEEK_SET);
            return false;
        }
        ++i;
    }
    return true;
}

bool expect_int(FILE* input, int* n) {
    char c = fgetc(input);
    *n = 0;
    bool ret = false;
    while (c >= '0' && c <= '9') {
        ret = true;
        *n = *n * 10 + (c - '0');
        c = fgetc(input);
    }
    ungetc(c, input);
    return ret;
}

int main() {
    FILE* input = fopen("data/day_03.txt", "r");

    int sum = 0;
    int sum_cond = 0;
    bool enable = true;

    int a, b;
    char c = fgetc(input);
    while (c != EOF) {
        if (expect_str(input, "do()"))
            enable = true;
        if (expect_str(input, "don't()"))
            enable = false;
        if (expect_str(input, "mul(")) {
            if (expect_int(input, &a) && expect_str(input, ",") && expect_int(input, &b) && expect_str(input, ")")) {
                int mul = a * b;
                sum += mul;
                if (enable)
                    sum_cond += mul;
            }
        } else {
            c = fgetc(input);
        }
    }
    fclose(input);

    printf("Part 1: %d\n", sum);
    printf("Part 2: %d\n", sum_cond);
}