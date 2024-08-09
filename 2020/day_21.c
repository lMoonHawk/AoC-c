#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap2.h"

#define LINE_MAX 1024

typedef struct {
    char* name;
    Hset* possible_ingredients;
} Allergen;


void hm_increment(Hmap* counter, void* key) {
    int* count = hm_search(*counter, key);
    if (count == NULL)
        hm_insert(counter, key, &(int) { 1 });
    else
        *count += 1;
}

void intersect_or_append(Allergen** allergens, Allergen* allergen) {
    bool found = false;
    for (size_t i = 0; i < da_length(*allergens); ++i) {
        if (strcmp(allergen->name, (*allergens)[i].name) == 0) {
            Hset_iter iter = hs_iter((*allergens)[i].possible_ingredients);
            char key[16];
            while (hs_iter_next(&iter, key)) {
                if (!hs_contains(*(allergen->possible_ingredients), key)) {
                    hs_remove((*allergens)[i].possible_ingredients, key);
                }
            }
            found = true;
            break;
        }
    }
    if (!found) {
        da_append(*allergens, *allergen);
    } else {
        free(allergen->name);
        hs_free(allergen->possible_ingredients);
    }
}

Allergen* parse_input(FILE* input, Hmap* counter) {
    Allergen* allergens = da_create(sizeof(*allergens));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Hset* possible_ingredients = hs_create(HM_STRING);

        char* tok = strtok(buffer, " ");
        while (tok[0] != '(') {
            hs_insert(possible_ingredients, tok);
            hm_increment(counter, tok);
            tok = strtok(NULL, " ");
        }
        char* allergen_name = strtok(NULL, "), ");
        while (allergen_name != NULL && allergen_name[0] != '\n' && allergen_name[0] != '\r') {
            Allergen allergen = { strdup(allergen_name), hs_copy(*possible_ingredients) };
            intersect_or_append(&allergens, &allergen);
            allergen_name = strtok(NULL, "), ");
        }
        hs_free(possible_ingredients);
    }
    return allergens;
}

void print_allergens(Allergen* allergens) {
    for (size_t i = 0; i < da_length(allergens); ++i) {
        printf("%-10s: ", allergens[i].name);
        Hset_iter iter = hs_iter(allergens[i].possible_ingredients);
        char key[16];
        while (hs_iter_next(&iter, key)) {
            printf("%s ", key);
        }
        printf("\n");
    }
    printf("\n");
}

int allergen_cmp(const void* a, const void* b) {
    return strcmp(((Allergen*)a)->name, ((Allergen*)b)->name);
}

int main() {
    FILE* input = fopen("data/day_21.txt", "r");
    Hmap* counter = hm_create(HM_STRING, sizeof(int));
    Allergen* allergens = parse_input(input, counter);
    fclose(input);

    int n = da_length(allergens);
    while (n != 0) {
        n = da_length(allergens);
        for (size_t i = 0; i < da_length(allergens); ++i) {
            if (hs_length(*(allergens[i].possible_ingredients)) == 1) {
                --n;
                continue;
            }
            for (size_t j = 0; j < da_length(allergens); ++j) {
                if (i == j)
                    continue;
                if (hs_length(*(allergens[j].possible_ingredients)) == 1) {
                    Hset_iter iter = hs_iter(allergens[j].possible_ingredients);
                    char key[16];
                    hs_iter_next(&iter, key);
                    hs_remove(allergens[i].possible_ingredients, key);
                }
            }
        }
    }
    qsort(allergens, da_length(allergens), sizeof(Allergen), allergen_cmp);
    char* dangerous = da_create(sizeof(char));
    for (size_t i = 0; i < da_length(allergens); ++i) {
        Hset_iter iter = hs_iter(allergens[i].possible_ingredients);
        char key[16];
        hs_iter_next(&iter, key);
        hm_remove(counter, key);
        da_extend(dangerous, key, strlen(key));
        da_append(dangerous, (char) { ',' });
    }
    da_remove(dangerous, da_length(dangerous) - 1);
    da_append(dangerous, (char) { '\0' });

    int safe_count = 0;
    Hmap_iter iter = hm_iter(counter);
    int value;
    while (hm_iter_next(&iter, NULL, &value))
        safe_count += value;

    for (size_t i = 0; i < da_length(allergens); ++i) {
        free(allergens[i].name);
        hs_free(allergens[i].possible_ingredients);
    }
    hs_free(counter);

    printf("Part 1: %d\n", safe_count);
    printf("Part 2: %s\n", dangerous);

    da_free(dangerous);
}