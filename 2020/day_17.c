#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CYCLES 6

typedef struct {
    int x, y, z, w;
} Point4;

bool* parse_input(FILE* input, int* w, int* h) {
    *w = 0;
    *h = 0;
    int capa = 9;
    bool* input_cubes = malloc(capa);
    int index = 0;
    int x = 0, y = 0;
    char c = fgetc(input);
    while (c != EOF) {
        if (c == '\n') {
            ++y;
            *w = x;
            x = 0;
        } else if (c != '\r') {
            bool value = c == '#';
            if (index == capa)
                input_cubes = realloc(input_cubes, capa *= 2);
            input_cubes[index++] = value;
            ++x;
            *h = y;
        }
        c = fgetc(input);
    }
    ++ * h;
    return input_cubes;
}

bool get_state(bool* cubes, Point4 p, Point4 dim) {
    int wh = dim.x * dim.y;
    int whd = wh * dim.z;
    return cubes[p.x + p.y * dim.x + p.z * wh + p.w * whd];
}

void set_state(bool* cubes, Point4 p, Point4 dim, bool value) {
    int wh = dim.x * dim.y;
    int whd = wh * dim.z;
    cubes[p.x + p.y * dim.x + p.z * wh + p.w * whd] = value;
}

int count_neighbours3d(bool* cubes, Point4 p, Point4 dim) {
    int active = 0;
    Point4 n = { 0, 0, 0, CYCLES };
    for (n.x = p.x - 1; n.x <= p.x + 1; ++n.x) {
        for (n.y = p.y - 1; n.y <= p.y + 1; ++n.y) {
            for (n.z = p.z - 1; n.z <= p.z + 1; ++n.z) {
                if (n.x == p.x && n.y == p.y && n.z == p.z && n.w == p.w)
                    continue;
                if (n.x < 0 || n.x >= dim.x || n.y < 0 || n.y >= dim.y || n.z < 0 || n.z >= dim.z)
                    continue;
                active += get_state(cubes, n, dim);
            }
        }
    }
    return active;
}
int count_neighbours4d(bool* cubes, Point4 p, Point4 dim) {
    int active = 0;
    Point4 n = { 0 };
    for (n.x = p.x - 1; n.x <= p.x + 1; ++n.x) {
        for (n.y = p.y - 1; n.y <= p.y + 1; ++n.y) {
            for (n.z = p.z - 1; n.z <= p.z + 1; ++n.z) {
                for (n.w = p.w - 1; n.w <= p.w + 1; ++n.w) {
                    if (n.x == p.x && n.y == p.y && n.z == p.z && n.w == p.w)
                        continue;
                    if (n.x < 0 || n.x >= dim.x || n.y < 0 || n.y >= dim.y || n.z < 0 || n.z >= dim.z || n.w < 0 || n.w >= dim.w)
                        continue;
                    active += get_state(cubes, n, dim);
                }
            }
        }
    }
    return active;
}

int count_active(bool* cubes_init, Point4 dim, int dim_count) {
    int size = dim.x * dim.y * dim.z * dim.w;
    bool* cubes = malloc(size * sizeof(*cubes));
    memcpy(cubes, cubes_init, size * sizeof(*cubes));
    bool* cubes_next = calloc(size, sizeof(*cubes_next));
    Point4 loop = { 0 };

    int wmin = 0;
    int wmax = dim.w - 1;
    if (dim_count == 3) {
        wmin = CYCLES;
        wmax = CYCLES;
    }
    for (int c = 0; c < CYCLES; ++c) {
        for (loop.w = wmin; loop.w <= wmax; ++loop.w) {
            for (loop.z = 0; loop.z < dim.z; ++loop.z) {
                for (loop.y = 0; loop.y < dim.y; ++loop.y) {
                    for (loop.x = 0; loop.x < dim.x; ++loop.x) {
                        int neigh = 0;
                        if (dim_count == 3)
                            neigh = count_neighbours3d(cubes, loop, dim);
                        else if (dim_count == 4)
                            neigh = count_neighbours4d(cubes, loop, dim);
                        if (neigh == 3)
                            set_state(cubes_next, loop, dim, true);
                        else if (neigh == 2)
                            set_state(cubes_next, loop, dim, get_state(cubes, loop, dim));
                        else
                            set_state(cubes_next, loop, dim, false);
                    }
                }
            }
        }
        bool* cubes_tmp = cubes;
        cubes = cubes_next;
        cubes_next = cubes_tmp;
    }
    free(cubes_next);

    int active = 0;
    for (int i = 0; i < size; ++i)
        active += cubes[i];
    free(cubes);

    return active;
}

int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    int init_width, init_height;
    bool* cubes_init = parse_input(input, &init_width, &init_height);
    fclose(input);

    /*
        4D array of the maxium possible size after CYCLES.
        The array expands by at most 2 for each dimensions per cycle.
        We don't want to deal with negative coordinates:
         we move the initial flat region to (CYCLES, CYCLES, CYCLES, CYCLES),
         thus the minimum possible index after CYCLES will be 0.
    */
    Point4 dim = {
        init_width + 2 * CYCLES,
        init_height + 2 * CYCLES,
        2 * CYCLES + 1,
        2 * CYCLES + 1
    };

    int wh = dim.x * dim.y;
    int whd = wh * dim.z;
    bool* cubes = calloc(whd * dim.w, sizeof(*cubes));
    for (int y = 0; y < init_height; ++y)
        for (int x = 0; x < init_width; ++x)
            cubes[x + CYCLES + (y + CYCLES) * dim.x + CYCLES * wh + CYCLES * whd] = cubes_init[x + y * init_width];
    free(cubes_init);

    int active3d = count_active(cubes, dim, 3);
    int active4d = count_active(cubes, dim, 4);
    free(cubes);

    printf("Part 1: %d\n", active3d);
    printf("Part 2: %d\n", active4d);
}