#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int ports[2];
    bool placed;
} Component;

void find_strongest(
    Component* components,
    int current_port,
    int strength, int length,
    int* strongest, int* rel_strongest, int* longest)
{
    for (size_t i = 0; i < da_length(components); ++i) {
        if (components[i].placed)
            continue;

        for (int p = 0; p < 2; ++p) {
            if (components[i].ports[p] == current_port) {
                int next_port = components[i].ports[1 - p];
                int new_strength = strength + current_port + next_port;
                int new_length = length + 1;

                if (new_strength > *strongest)
                    *strongest = new_strength;
                if (new_length > *longest) {
                    *longest = length;
                    *rel_strongest = new_strength;
                } else if (new_length == *longest) {
                    if (new_strength > *rel_strongest) 
                        *rel_strongest = new_strength;
                }

                components[i].placed = true;
                find_strongest(components, next_port, new_strength, new_length, strongest, rel_strongest, longest);
                components[i].placed = false;
            }
        }
    }
}

Component* parse_input(FILE* input) {
    Component* components = da_create(sizeof(*components));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Component component = { { atoi(strtok(buffer, "/")), atoi(strtok(NULL, " ")) }, false };
        da_append(components, component);
    }
    return components;
}

int main() {
    FILE* input = fopen("data/day_24.txt", "r");
    Component* components = parse_input(input);
    fclose(input);

    int strongest = 0, longest, rel_strongest;
    find_strongest(components, 0, 0, 0, &strongest, &rel_strongest, &longest);
    da_free(components);

    printf("Part 1: %d\n", strongest);
    printf("Part 2: %d\n", rel_strongest);
}