#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int depth, range, cycle;
} Layer;

bool cross_is_safe(Layer* layers, int delay, int*score) {
    for (size_t i = 0; i < da_length(layers); ++i) {
        if ((layers[i].depth + delay) % layers[i].cycle == 0) {
            if (score == NULL)
                return 0;
            *score += layers[i].depth * layers[i].range;
        }
    }
    return 1;
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");

    Layer* layers = da_create(sizeof(*layers));

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Layer layer = { 0 };
        layer.depth = atoi(strtok(buffer, ": "));
        layer.range = atoi(strtok(NULL, ": "));
        layer.cycle = 2 * layer.range - 2;
        da_append(layers, layer);
    }
    fclose(input);

    int score = 0;
    cross_is_safe(layers, 0, &score);
    int delay = 1;
    while (!cross_is_safe(layers, delay++, NULL));

    da_free(layers);

    printf("Part 1: %d\n", score);
    printf("Part 2: %d\n", delay - 1);
}