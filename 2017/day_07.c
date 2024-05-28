#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"


#define LINE_MAX 256

typedef struct {
    char name[16];
    int weight;
    int tot_weight;
    int* holds;
} Program;

int get_index(Program* pgs, const char* name) {
    for (size_t i = 0; i < da_length(pgs); ++i) {
        if (strcmp(pgs[i].name, name) == 0)
            return i;
    }
    return -1;
}

// Find the bottom program by exclusion
int get_index_bottom_pg(Program* pgs) {
    size_t n = da_length(pgs);
    bool is_bottom_pg[n];
    memset(is_bottom_pg, true, sizeof(is_bottom_pg));

    for (size_t i = 0; i < n; ++i) {
        if (pgs[i].holds != NULL) {
            for (size_t j = 0; j < da_length(pgs[i].holds); ++j) {
                is_bottom_pg[pgs[i].holds[j]] = false;
            }
        }
    }
    for (size_t i = 0; i < n; ++i) {
        if (is_bottom_pg[i])
            return i;
    }
    return -1;
}

// Recursively evaluates the total weights of all children
int resolve_weight(Program* pgs, int index, int* inbalance, int* unbalanced) {

    int tot_weight = pgs[index].weight;
    int* sub_pgs = pgs[index].holds;

    if (sub_pgs != NULL) {
        int sub_cnt = da_length(sub_pgs);

        int weight_1 = resolve_weight(pgs, sub_pgs[0], inbalance, unbalanced);
        tot_weight += weight_1;
        int weight_1_cnt = 1;
        int weight_2 = 0;

        for (int i = 1; i < sub_cnt; ++i) {
            int weight = resolve_weight(pgs, sub_pgs[i], inbalance, unbalanced);
            tot_weight += weight;

            if (weight != weight_1) {
                weight_2 = weight;
                if (*inbalance == 0) *unbalanced = sub_pgs[i];
            } else {
                ++weight_1_cnt;
            }
        }
        // First inbalance found is the cause
        if (*inbalance == 0 && weight_1_cnt < sub_cnt) {
            if (weight_1_cnt == sub_cnt - 1) {
                *inbalance = weight_1 - weight_2;
            } else if (weight_1_cnt == 1) {
                *inbalance = weight_2 - weight_1;
                *unbalanced = sub_pgs[0];
            }
        }
    }
    return tot_weight;
}

Program* parse_input(FILE* input) {
    Program* out = da_create(sizeof(*out));
    char** child_table = da_create(sizeof(*child_table));

    // 2 passes to resolve children indexes
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Program current = { 0 };
        char* tok = strtok(buffer, " ");
        strcpy(current.name, tok);
        tok = strtok(NULL, ")");
        ++tok;
        current.weight = atoi(tok);

        da_append(out, current);

        strtok(NULL, " ");
        char* children_str;
        if ((tok = strtok(NULL, "\r\n")) != NULL) {
            children_str = strdup(tok);
        } else {
            children_str = NULL;
        }
        da_append(child_table, children_str);
    }

    for (size_t i = 0; i < da_length(out); ++i) {
        if (child_table[i] != NULL) {
            out[i].holds = da_create(sizeof(int));

            char* child = strtok(child_table[i], ", ");
            while (child != NULL) {
                int index = get_index(out, child);
                da_append(out[i].holds, index);
                child = strtok(NULL, ", ");
            }
            free(child_table[i]);
        }
    }

    da_free(child_table);
    return out;
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");
    Program* pgs = parse_input(input);
    fclose(input);

    int bottom = get_index_bottom_pg(pgs);
    int inbalance = 0, index_unbalanced = -1;
    resolve_weight(pgs, bottom, &inbalance, &index_unbalanced);

    for (size_t i = 0; i < da_length(pgs); ++i) {
        if (pgs[i].holds != NULL) da_free(pgs[i].holds);
    }
    da_free(pgs);

    printf("Part 1: %s\n", pgs[bottom].name);
    printf("Part 2: %d\n", pgs[index_unbalanced].weight + inbalance);
}