#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int index;
    bool is_subrule;
    union {
        char letter;
        struct {
            int sublen1, sublen2;
            int subrule1[3];
            int subrule2[3];
        };
    };
} Rule;

typedef struct {
    int si;
    int* next_rules;
} State;

void parse_input(FILE* input, char*** messages, Rule** rules) {
    char buffer[LINE_MAX] = { 0 };
    // Rules
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Rule rule = { 0 };
        rule.index = atoi(strtok(buffer, ":"));
        char* token = strtok(NULL, " ");
        if (token[0] == '"') {
            rule.is_subrule = false;
            rule.letter = token[1];
        } else {
            rule.is_subrule = true;
            while (token != NULL && token[0] != '|') {
                rule.subrule1[rule.sublen1++] = atoi(token);
                token = strtok(NULL, " ");
            }
            if (token != NULL) {
                token = strtok(NULL, " ");
                while (token != NULL) {
                    rule.subrule2[rule.sublen2++] = atoi(token);
                    token = strtok(NULL, " ");
                }
            }
        }
        da_append(*rules, rule);
    }

    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        buffer[strcspn(buffer, "\n\r")] = '\0';
        da_append(*messages, (char*) { strdup(buffer) });
    }
}

int compare_rule_index(const void* a, const void* b) {
    return ((const Rule*)a)->index - ((const Rule*)b)->index;
}

bool is_matching(const char* s, Rule* rules) {
    int n = strlen(s);

    State* stack = da_create(sizeof(State));
    State init = { 0, da_create(sizeof(int)) };
    da_append(init.next_rules, (int) { 0 });
    da_append(stack, init);
    State current, new_state;
    int next_rule_index;
    bool match = false;
    while (da_length(stack)) {
        da_pop(stack, &current);

        if (current.si == n || da_length(current.next_rules) == 0) {
            match = (current.si == n && da_length(current.next_rules) == 0);
            da_free(current.next_rules);
            if (match)
                break;
            continue;
        };

        da_pop(current.next_rules, &next_rule_index);
        Rule next_rule = rules[next_rule_index];

        if (!next_rule.is_subrule) {
            if (s[current.si] != next_rule.letter) {
                da_free(current.next_rules);
                continue;
            }
            new_state = (State){ current.si + 1, da_copy(current.next_rules) };
            da_append(stack, new_state);
        } else {
            new_state = (State){ current.si, da_copy(current.next_rules) };
            for (int i = next_rule.sublen1 - 1; i >= 0; --i)
                da_append(new_state.next_rules, (int) { next_rule.subrule1[i] });
            da_append(stack, new_state);
            if (next_rule.sublen2 > 0) {
                new_state = (State){ current.si, da_copy(current.next_rules) };
                for (int i = next_rule.sublen2 - 1; i >= 0; --i)
                    da_append(new_state.next_rules, (int) { next_rule.subrule2[i] });
                da_append(stack, new_state);
            }
        }
        da_free(current.next_rules);
    }
    for (size_t i = 0; i < da_length(stack); ++i)
        da_free(stack[i].next_rules);
    da_free(stack);
    return match;
}


int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    char** messages = da_create(sizeof(char*));
    Rule* rules = da_create(sizeof(Rule));
    parse_input(input, &messages, &rules);
    fclose(input);
    qsort(rules, da_length(rules), sizeof(Rule), compare_rule_index);

    int match_count_p1 = 0;
    for (size_t i = 0; i < da_length(messages); ++i) {
        match_count_p1 += is_matching(messages[i], rules);
    }

    rules[8].sublen2 = 2;
    rules[8].subrule2[0] = 42;
    rules[8].subrule2[1] = 8;

    rules[11].sublen2 = 3;
    rules[11].subrule2[0] = 42;
    rules[11].subrule2[1] = 11;
    rules[11].subrule2[2] = 31;

    int match_count_p2 = 0;
    for (size_t i = 0; i < da_length(messages); ++i) {
        match_count_p2 += is_matching(messages[i], rules);
        free(messages[i]);
    }
    da_free(rules);
    da_free(messages);

    printf("Part 1: %d\n", match_count_p1);
    printf("Part 2: %d\n", match_count_p2);
}