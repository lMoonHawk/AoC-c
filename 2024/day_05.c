#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

void parse_input(FILE* input, int** rules, int*** updates) {

    char buffer[LINE_MAX] = { 0 };

    int pages[2] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%d|%d", &pages[0], &pages[1]);
        da_extend(*rules, pages, 2);
    }

    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int* update = da_create(sizeof(int));

        char* tok = strtok(buffer, ",");
        while (tok != NULL) {
            da_append(update, (int) { atoi(tok) });
            tok = strtok(NULL, ",");
        }
        da_append(*updates, update);
    }

}

int main() {
    FILE* input = fopen("data/day_05.txt", "r");
    int* rules = da_create(sizeof(int));
    int** updates = da_create(sizeof(int*));
    parse_input(input, &rules, &updates);

    fclose(input);

    int sum_mid_valid = 0;
    int sum_mid = 0;
    for (size_t i = 0; i < da_length(updates); ++i) {
        bool valid = true;
        for (size_t j = 1; j < da_length(updates[i]); ++j) {
            // For each page in the update, check all rules and move the page if it should be after
            for (size_t k = 0; k < da_length(rules); k += 2) {
                if (updates[i][j] == rules[k]) {
                    // Check all previous page for the relevant page in the rule
                    for (size_t l = 0; l < j; ++l) {
                        if (updates[i][l] == rules[k + 1]) {
                            valid = false;
                            da_remove(updates[i], l);
                            da_insert(updates[i], rules[k + 1], j--);
                            break;
                        }
                    }
                }
            }
        }
        int mid = updates[i][(da_length(updates[i]) - 1) / 2];
        sum_mid_valid += valid * mid;
        sum_mid += mid;
    }


    da_free(rules);
    for (size_t i = 0; i < da_length(updates); ++i)
        da_free(updates[i]);
    da_free(updates);

    printf("Part 1: %d\n", sum_mid_valid);
    printf("Part 2: %d\n", sum_mid - sum_mid_valid);
}