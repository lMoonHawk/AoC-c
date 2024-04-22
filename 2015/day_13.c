#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define NAME_MAX 16

typedef struct {
    char name[NAME_MAX];
    char other[NAME_MAX];
    int happiness;
} Lookup;

int da_indexof(void* da, void* value) {
    size_t len = da_length(da);
    size_t size = da_unit_size(da);
    char* ptr = (char*)da;

    for (size_t i = 0; i < len; ++i) {
        if (memcmp(ptr + i * size, value, size) == 0) {
            return i;
        }
    }
    return -1;
}

int da_strindexof(char** da, void* value) {
    size_t len = da_length(da);
    for (size_t i = 0; i < len; ++i) {
        if (strcmp(da[i], value) == 0) {
            return i;
        }
    }
    return -1;
}

void swap_c(char** a, char** b) {
    char* temp = *a;
    *a = *b;
    *b = temp;
}

int get_happiness(const char* k1, const char* k2, Lookup* rships, size_t nb_names) {
    for (size_t j = 0; j < nb_names * (nb_names - 1); j++) {
        if (strcmp(k1, rships[j].name) == 0 && strcmp(k2, rships[j].other) == 0) {
            return rships[j].happiness;
        }
    }
    return 0;
}

int path_happiness(char** names, Lookup* rships, size_t nb_names) {
    int total = 0;
    for (size_t i = 0; i < nb_names; i++) {
        size_t next_i = i < (nb_names - 1) ? i + 1 : 0;
        total += get_happiness(names[i], names[next_i], rships, nb_names);
        total += get_happiness(names[next_i], names[i], rships, nb_names);
    }
    return total;
}

int get_best_happiness(char** names, size_t start, size_t end, Lookup* dists) {
    int result = INT_MIN;
    if (start == end) return path_happiness(names, dists, end + 1);
    else {
        for (size_t i = start; i <= end; i++) {
            swap_c(&names[start], &names[i]);
            int current = get_best_happiness(names, start + 1, end, dists);
            if (current > result) result = current;
            swap_c(&names[start], &names[i]);
        }
    }
    return result;
}

void parse_input(FILE* input, char*** names, Lookup** rships) {
    char buffer[LINE_MAX];
    char* name, * other;
    int sign, happiness;

    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        name = strtok(buffer, " ");
        strtok(NULL, " ");
        sign = 2 * (strcmp(strtok(NULL, " "), "gain") == 0) - 1;
        happiness = sign * atoi(strtok(NULL, " "));
        for (int i = 0; i < 6; i++) strtok(NULL, " ");
        other = strtok(NULL, ".");

        Lookup entry = { 0 };
        strcpy(entry.name, name);
        strcpy(entry.other, other);
        entry.happiness = happiness;
        da_append(*rships, entry);

        char* name_cpy = strdup(name);
        if (da_strindexof(*names, name_cpy) == -1) da_append(*names, name_cpy);
    }
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");

    char** names = da_create(sizeof(char*));
    Lookup* rships = da_create(sizeof(Lookup));

    parse_input(input, &names, &rships);

    int max = get_best_happiness(names, 0, da_length(names) - 1, rships);

    char* new = " ";
    for (size_t k = 0; k < da_length(names); ++k) {
        Lookup entry = { 0 };
        strcpy(entry.name, names[k]);
        strcpy(entry.other, new);
        entry.happiness = 0;
    }
    da_append(names, new);
    int max2 = get_best_happiness(names, 0, da_length(names) - 1, rships);

    printf("Part 1: %d\n", max);
    printf("Part 2: %d\n", max2);

    da_free(names);
    da_free(rships);
    fclose(input);
}