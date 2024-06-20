#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define LINE_MAX 256
#define GENERATIONS 50000000000

static inline int get_first_plant_i(char* plants) {
    return (int)(strchr(plants, '#') - plants);
}

Hmap* parse_input(FILE* input, char* plants, size_t* n) {
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    strtok(buffer, " ");
    strtok(NULL, " ");

    // add padding of 4 pots on each side
    char* ini_state = strtok(NULL, "\r\n");
    *n = strlen(ini_state);
    strncpy(plants + 4, ini_state, *n);
    *n += 8;
    *(plants + *n) = '\0';

    fgets(buffer, sizeof(buffer), input);
    Hmap* cases = hm_create(5, 1);
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* key = strtok(buffer, " ");
        strtok(NULL, " ");
        char value = strtok(NULL, " ")[0];
        hm_insert(cases, key, &value);
    }
    return cases;
}

int main() {
    FILE* input = fopen("data/day_12.txt", "r");

    char* plants = malloc(LINE_MAX);
    memset(plants, '.', LINE_MAX);
    size_t n;
    Hmap* cases = parse_input(input, plants, &n);
    fclose(input);


    int64_t plant_values_20 = 0;
    int first_plant = get_first_plant_i(plants);
    uint64_t k = 0;
    int drift_amount = 0;
    bool cycle = false;
    for (k = 0; k < GENERATIONS && !cycle; ++k) {
        n += 8;
        char* new_plants = malloc(n + 1);
        for (size_t i = 0; i < n; ++i) {
            if (i < 6 || i > n - 6 - 1) {
                new_plants[i] = '.';
            } else {
                char* val = hm_search(*cases, plants + i - 6);
                new_plants[i] = *val;
            }
        }
        new_plants[n] = '\0';

        int new_first_plant = get_first_plant_i(new_plants);
        if (strncmp(plants + first_plant, new_plants + new_first_plant, n - 8 - first_plant) == 0) {
            cycle = true;
            drift_amount = new_first_plant - first_plant - 4;
        }
        char* tmp = plants;
        plants = new_plants;
        free(tmp);
        first_plant = new_first_plant;

        if (k == 19) {
            for (size_t i = first_plant; i < n; ++i)
                if (plants[i] == '#')
                    plant_values_20 += -4 * (k + 2) + i;
        }
    }
    hm_free(cases);

    int64_t plant_num_index_0 = -4 * (k + 1);
    int64_t drift_amount_total = (GENERATIONS - k) * drift_amount;
    int64_t plant_values = 0;
    for (size_t i = first_plant; i < n; ++i)
        if (plants[i] == '#')
            plant_values += (plant_num_index_0 + i) + drift_amount_total;

    free(plants);

    printf("Part 1: %ld\n", plant_values_20);
    printf("Part 2: %ld\n", plant_values);
}