#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define NAME_MAX 16

typedef struct {
    char name[NAME_MAX];
    uint8_t count;
} Bag;

Hmap* parse_input(FILE* input) {
    Hmap* bags = hm_create(HM_STRING, sizeof(Bag*));
    int line = 0;
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        ++line;
        char* adj = strtok(buffer, " ");
        char* color = strtok(NULL, " ");
        char name[NAME_MAX] = { 0 };
        strncpy(name, adj, NAME_MAX);
        strncat(name, color, NAME_MAX - strlen(adj) - 1);
        Bag* inside = da_create(sizeof(Bag));
        strtok(NULL, " ");
        strtok(NULL, " ");

        Bag bag = { 0 };
        bag.count = atoi(strtok(NULL, " "));
        while (bag.count != 0) {
            adj = strtok(NULL, " ");
            color = strtok(NULL, " ");
            strncpy(bag.name, adj, NAME_MAX);
            strncat(bag.name, color, NAME_MAX - strlen(adj) - 1);
            da_append(inside, bag);
            bag.name[0] = '\0';
            strtok(NULL, " ");
            char* count_str = strtok(NULL, " ");
            bag.count = count_str == NULL ? 0 : atoi(count_str);
        }
        hm_insert(bags, name, &inside);
    }
    return bags;
}

bool has_shiny_gold(Hmap* bags, char* name) {
    if (strcmp(name, "shinygold") == 0)
        return true;
    Bag** inside = hm_search(*bags, name);
    for (size_t i = 0; i < da_length(*inside); ++i) {
        if (has_shiny_gold(bags, (*inside)[i].name))
            return true;
    }
    return false;
}

int count_bags(Hmap* bags, char* name) {
    Bag** inside = hm_search(*bags, name);
    if (da_length(*inside) == 0)
        return 1;
    int out = 0;
    for (size_t i = 0; i < da_length(*inside); ++i)
        out += (*inside)[i].count * count_bags(bags, (*inside)[i].name);
    return out + 1;
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");
    Hmap* bags = parse_input(input);
    fclose(input);

    int has_shinygold_cnt = -1;
    for (size_t i = 0; i < bags->capacity; ++i) {
        if (bags->htable[i].key != NULL) {
            if (has_shiny_gold(bags, bags->htable[i].key))
                ++has_shinygold_cnt;
        }
    }
    int req_bags_cnt = count_bags(bags, "shinygold") - 1;

    for (size_t i = 0; i < bags->capacity; ++i) {
        if (bags->htable[i].key != NULL) {
            da_free(*(Bag**)bags->htable[i].value);
        }
    }
    hs_free(bags);

    printf("Part 1: %d\n", has_shinygold_cnt);
    printf("Part 2: %d\n", req_bags_cnt);
}