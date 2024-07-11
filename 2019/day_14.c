#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"

#define LINE_MAX 256

#define TOTAL_ORE 1000000000000

typedef struct {
    char* name;
    int64_t amount;
} Chemical;

typedef struct {
    Chemical output;
    Chemical* input;
} Recipe;

static inline int mod(int a, int b) {
    return (a % b + b) % b;
}

Recipe* find_recipie(Recipe* recipies, char* name) {
    for (size_t i = 0; i < da_length(recipies); ++i) {
        if (strcmp(recipies[i].output.name, name) == 0)
            return &recipies[i];
    }
    return NULL;
}

int get_surplus(Chemical* surplus, char* name) {
    for (size_t i = 0; i < da_length(surplus); ++i) {
        if (strcmp(surplus[i].name, name) == 0)
            return surplus[i].amount;
    }
    return 0;
}

void set_surplus(Chemical** surplus, char* name, int amount) {
    for (size_t i = 0; i < da_length(*surplus); ++i) {
        if (strcmp((*surplus)[i].name, name) == 0) {
            (*surplus)[i].amount = amount;
            return;
        }
    }
    da_append(*surplus, ((Chemical){ strdup(name), amount }));
}

int64_t get_required_ore_amt(Recipe* recipies, int64_t fuel_amt) {
    Chemical* surplus = da_create(sizeof(Chemical));
    Queue q = q_create(sizeof(Chemical));
    q_push(q, ((Chemical){"FUEL", fuel_amt}));
    int64_t required_ore_amt = 0;
    while (q_length(q)) {
        Chemical output;
        q_pop(q, &output);
        if (strcmp(output.name, "ORE") == 0) {
            required_ore_amt += output.amount;
            continue;
        }
        Recipe* input = find_recipie(recipies, output.name);
        int64_t needed = output.amount - get_surplus(surplus, output.name);
        int64_t reaction_count = needed / input->output.amount;
        int64_t remainder = mod(needed, input->output.amount);
        if (needed < 0)
            reaction_count--;

        if (remainder > 0) {
            ++reaction_count;
            set_surplus(&surplus, output.name, input->output.amount - remainder);
        } else {
            set_surplus(&surplus, output.name, 0);
        }

        for (size_t i = 0; i < da_length(input->input); ++i) {
            int64_t needed = reaction_count * input->input[i].amount;
            q_push(q, ((Chemical){input->input[i].name, needed}));
        }
    }
    q_free(q);
    for (size_t i = 0; i < da_length(surplus); ++i)
        free(surplus[i].name);
    da_free(surplus);
    return required_ore_amt;
}

void free_recipies(Recipe* recipies) {
    for (size_t i = 0; i < da_length(recipies); ++i) {
        for (size_t j = 0; j < da_length(recipies[i].input); ++j) {
            free(recipies[i].input[j].name);
        }
        da_free(recipies[i].input);
        free(recipies[i].output.name);
    }
    da_free(recipies);
}

Recipe* parse_input(FILE* input) {
    Recipe* recipies = da_create(sizeof(Recipe));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Recipe recipe = { 0 };
        Chemical* input = da_create(sizeof(Chemical));
        char* inputs = strtok(buffer, "=");
        char* output = strtok(NULL, "\r\n") + 2;

        recipe.output.amount = atoi(strtok(output, " "));
        recipe.output.name = strdup(strtok(NULL, "\r\n"));

        int input_amount = atoi(strtok(inputs, " "));
        char* input_name = strtok(NULL, ", ");
        da_append(input, ((Chemical) { strdup(input_name), input_amount }));
        char* tok;
        while ((tok = strtok(NULL, " ")) != NULL) {
            input_amount = atoi(tok);
            input_name = strtok(NULL, ", ");
            da_append(input, ((Chemical) { strdup(input_name), input_amount }));
        }
        recipe.input = input;
        da_append(recipies, recipe);
    }
    return recipies;
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");
    Recipe* recipies = parse_input(input);
    fclose(input);

    int64_t ore_for_one_fuel = get_required_ore_amt(recipies, 1);

    int64_t prev_candidate = 0;
    int64_t candidate = TOTAL_ORE / ore_for_one_fuel;
    while (prev_candidate != candidate) {
        int64_t diff = TOTAL_ORE - get_required_ore_amt(recipies, candidate);
        prev_candidate = candidate;
        candidate += diff / ore_for_one_fuel;
    }
    free_recipies(recipies);

    printf("Part 1: %ld\n", ore_for_one_fuel);
    printf("Part 2: %ld\n", candidate);
}