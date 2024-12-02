#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

bool check_seq(int* report, int skip_idx) {
    bool incr = report[1] > report[0];
    if (skip_idx == 0)
        incr = report[2] > report[1];
    else if (skip_idx == 1)
        incr = report[2] > report[0];

    for (int i = 1; i < (int)da_length(report); ++i) {
        if ((i == skip_idx) | ((i == 1) & (skip_idx == 0)))
            continue;
        int prev_idx = i - 1;
        if (i == skip_idx + 1)
            prev_idx = i - 2;

        int delta = abs(report[i] - report[prev_idx]);
        if (((report[i] > report[prev_idx]) != incr) | (delta == 0) | (delta > 3)) {
            return false;
        }
    }
    return true;
}

int main() {
    FILE* input = fopen("data/day_02.txt", "r");
    char buffer[LINE_MAX] = { 0 };

    int* report = da_create(sizeof(int));

    int safe_cnt = 0;
    int safe_tol_cnt = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* tok = strtok(buffer, " \r\n");
        while (tok != NULL) {
            da_append(report, (int) { atoi(tok) });
            tok = strtok(NULL, " \r\n");
        }
        if (check_seq(report, -1)) {
            ++safe_cnt;
            ++safe_tol_cnt;
        } else {
            for (size_t i = 0; i < da_length(report); ++i) {
                if (check_seq(report, i)) {
                    ++safe_tol_cnt;
                    break;
                }
            }
        }

        da_set_length(report, 0);
    }
    fclose(input);
    da_free(report);

    printf("Part 1: %d\n", safe_cnt);
    printf("Part 2: %d\n", safe_tol_cnt);
}