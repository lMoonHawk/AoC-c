#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    char* name;
    int min1, max1;
    int min2, max2;
} Rule;

int* parse_ticket(char* ticket_str, size_t fields_count) {
    int* ticket = malloc(fields_count * sizeof(*ticket));
    ticket[0] = atoi(strtok(ticket_str, ","));
    for (size_t i = 1; i < fields_count; ++i)
        ticket[i] = atoi(strtok(NULL, ","));
    return ticket;
}

void parse_input(FILE* input, Rule** rules, int** ticket, int*** nearby_tickets) {
    *rules = da_create(sizeof(Rule));
    *nearby_tickets = da_create(sizeof(int*));

    char buffer[LINE_MAX] = { 0 };
    // Rules
    Rule rule = { 0 };
    while (fgets(buffer, LINE_MAX, input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        rule.name = strdup(strtok(buffer, ":"));
        rule.min1 = atoi(strtok(NULL, " -"));
        rule.max1 = atoi(strtok(NULL, " -"));
        strtok(NULL, " ");
        rule.min2 = atoi(strtok(NULL, " -"));
        rule.max2 = atoi(strtok(NULL, " -"));
        da_append(*rules, rule);
    }
    fgets(buffer, LINE_MAX, input);
    // Own ticket
    fgets(buffer, LINE_MAX, input);
    *ticket = parse_ticket(buffer, da_length(*rules));
    fgets(buffer, LINE_MAX, input);
    // Nearby tickets
    fgets(buffer, LINE_MAX, input);
    while (fgets(buffer, LINE_MAX, input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int* nearby_ticket = parse_ticket(buffer, da_length(*rules));
        da_append(*nearby_tickets, nearby_ticket);
    }
}

bool is_field_rule_valid(int value, Rule rule) {
    if (value >= rule.min1 && value <= rule.max1)
        return true;
    else if (value >= rule.min2 && value <= rule.max2)
        return true;
    return false;
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    Rule* rules = NULL;
    int* ticket = NULL;
    int** nearby_tickets = NULL;
    parse_input(input, &rules, &ticket, &nearby_tickets);
    fclose(input);

    int field_count = da_length(rules);
    int ticket_count = da_length(nearby_tickets);

    int error_rate = 0;
    for (int i = 0; i < ticket_count; ++i) {
        bool remove = false;
        for (int j = 0; j < field_count; ++j) {
            bool field_valid = false;
            for (int k = 0; k < field_count; ++k) {
                if (is_field_rule_valid(nearby_tickets[i][j], rules[k])) {
                    field_valid = true;
                    break;
                }
            }
            if (!field_valid) {
                error_rate += nearby_tickets[i][j];
                remove = true;
            }
        }
        if (remove) {
            free(nearby_tickets[i]);
            da_remove(nearby_tickets, i--);
            --ticket_count;
        }
    }

    // possible[i][j] : ticket position i could be actual position j
    bool(*possible)[field_count] = malloc(field_count * sizeof(*possible));
    memset(possible, true, field_count * field_count);
    // actual_position[k] : gives actual position for ticket position k
    int* actual_positions = malloc(field_count * sizeof(*actual_positions));
    memset(actual_positions, -1, field_count * sizeof(*actual_positions));

    for (int i = 0; i < ticket_count; ++i) {
        for (int j = 0; j < field_count; ++j) {
            for (int k = 0; k < field_count; ++k) {
                if (!is_field_rule_valid(nearby_tickets[i][j], rules[k]))
                    possible[j][k] = false;
            }
        }
        free(nearby_tickets[i]);
    }
    da_free(nearby_tickets);

    int found = 0;
    while (found < field_count) {
        for (int i = 0; i < field_count; ++i) {
            if (actual_positions[i] != -1)
                continue;
            int actual_position = -1;
            int possible_count = 0;
            for (int j = 0; j < field_count; ++j) {
                if (possible[i][j]) {
                    ++possible_count;
                    actual_position = j;
                }
            }
            if (possible_count == 1) {
                ++found;
                actual_positions[i] = actual_position;
                for (int j = 0; j < field_count; ++j)
                    possible[j][actual_position] = false;
            }
        }
    }
    free(possible);

    int64_t result = 1;
    int n = strlen("departure");
    for (int i = 0; i < field_count; ++i)
        if (strncmp(rules[actual_positions[i]].name, "departure", n) == 0)
            result *= ticket[i];
    free(actual_positions);
    free(ticket);

    for (size_t i = 0; i < da_length(rules); ++i)
        free(rules[i].name);
    da_free(rules);

    printf("Part 1: %d\n", error_rate);
    printf("Part 2: %" PRId64 "\n", result);
}