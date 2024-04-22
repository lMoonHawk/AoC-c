#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

enum Property {
    CAPACITY,
    DURABILITY,
    FLAVOR,
    TEXTURE,
    CALORIES,
    PROPERTIES_CNT,
};

unsigned long score(int calories, size_t ing_cnt, int recipe[ing_cnt], int** ingredients) {
    unsigned long out = 1;

    if (calories >= 0) {
        long cookie_calories = 0;
        for (size_t j = 0; j < ing_cnt; ++j) {
            cookie_calories += recipe[j] * ingredients[j][CALORIES];
        }
        if (cookie_calories != calories) return 0;
    }
    for (int i = 0; i < CALORIES; ++i) {
        long prop_score = 0;
        for (size_t j = 0; j < ing_cnt; ++j) {
            prop_score += recipe[j] * ingredients[j][i];
        }
        if (prop_score < 0) out = 0;
        out *= prop_score;
    }
    return out;
}

unsigned long best_recipe(int calories, size_t ing_cnt, int recipe[ing_cnt], int** ingredients, size_t index, int subtotal) {
    if (index == ing_cnt - 1) {
        recipe[index] = 100 - subtotal;
        return score(calories, ing_cnt, recipe, ingredients);
    }
    unsigned long max = 0;
    for (int k = 0; k <= 100; ++k) {
        if (subtotal + k > 100) break;

        recipe[index] = k;
        unsigned long current = best_recipe(calories, ing_cnt, recipe, ingredients, index + 1, subtotal + k);
        if (current > max) max = current;
    }
    return max;
}

void parse_input(FILE* input, int*** ingredients) {
    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int* properties = malloc(5 * sizeof(int));
        strtok(buffer, " ");
        for (int p = 0; p < PROPERTIES_CNT; ++p) {
            strtok(NULL, " ");
            properties[p] = atoi(strtok(NULL, " "));
        }
        da_append(*ingredients, properties);
    }
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");
    int** ingredients = da_create(sizeof(int*));

    parse_input(input, &ingredients);

    size_t n = da_length(ingredients);
    int recipe[n];
    printf("Part 1: %lu\n", best_recipe(-1, n, recipe, ingredients, 0, 0));
    printf("Part 2: %lu\n", best_recipe(500, n, recipe, ingredients, 0, 0));

    for (size_t i = 0; i < da_length(ingredients); ++i) free(ingredients[i]);
    da_free(ingredients);
    fclose(input);
}