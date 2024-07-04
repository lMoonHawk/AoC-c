#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define WIDTH 25
#define HEIGHT 6

void* parse_input(FILE* input) {
    uint8_t* layers = da_create(sizeof(uint8_t));
    char c = fgetc(input);
    while (c != '\n' && c != '\r' && c != EOF) {
        da_append(layers, (uint8_t) { c - '0' });
        c = fgetc(input);
    }
    return layers;
}

int main() {
    FILE* input = fopen("data/day_08.txt", "r");
    uint8_t(*layers)[HEIGHT][WIDTH] = parse_input(input);
    fclose(input);
    int layers_cnt = da_length(layers) / HEIGHT / WIDTH;

    int counter[3] = { 0 };
    int fewest_0s = INT_MAX;

    for (int l = 0; l < layers_cnt; ++l) {
        int layer_counter[3] = { 0 };
        int layer_0s = 0;
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                layer_0s += layers[l][y][x] == 0;
                ++layer_counter[layers[l][y][x]];
            }
        }
        if (layer_0s < fewest_0s) {
            fewest_0s = layer_0s;
            for (int i = 0; i < 3; ++i)
                counter[i] = layer_counter[i];
        }
    }

    char output[2 * (WIDTH * HEIGHT) + HEIGHT + 1] = { 0 };
    char* p = output;
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            for (int l = 0; l < layers_cnt; ++l) {
                if (layers[l][y][x] == 2)
                    continue;
                *p++ = layers[l][y][x] == 1 ? '#' : ' ';
                *p++ = layers[l][y][x] == 1 ? '#' : ' ';
                break;
            }
        }
        *p++ = '\n';
    }
    *(--p) = '\0';

    da_free(layers);

    printf("Part 1: %d\n", counter[1] * counter[2]);
    printf("Part 2: \n%s\n", output);
}