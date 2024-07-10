#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256
#define NB_MOONS 4

static inline bool is_cycle(Point3* pos, Point3* pos_init, Point3* vel, Point3* vel_init) {
    return memcmp(pos, pos_init, NB_MOONS * sizeof(Point3)) == 0 && memcmp(vel, vel_init, NB_MOONS * sizeof(Point3)) == 0;
}

void simulate_axis(Point3* moons_pos, Point3* moons_vel, int axis) {
    for (int i = 0; i < NB_MOONS; ++i) {
        for (int j = i + 1; j < NB_MOONS; ++j) {
            if (moons_pos[i].components[axis] != moons_pos[j].components[axis]) {
                int i_gt_j = 2 * (moons_pos[i].components[axis] > moons_pos[j].components[axis]) - 1;
                moons_vel[i].components[axis] -= i_gt_j;
                moons_vel[j].components[axis] += i_gt_j;
            }
        }
    }
    for (int i = 0; i < NB_MOONS; ++i)
        moons_pos[i].components[axis] += moons_vel[i].components[axis];
}

int gcd(uint64_t combin[3]) {
    uint64_t gcd = 0;
    for (int i = 0; i < 3; ++i) {
        uint64_t a = gcd;
        uint64_t b = combin[i];
        while (b) {
            uint64_t t = b;
            b = a % b;
            a = t;
        }
        gcd = a;
    }
    return gcd;
}

uint64_t lcm(uint64_t cycles[3]) {
    uint64_t num = 1;
    for (int i = 0; i < 3; ++i)
        num *= cycles[i];
    uint64_t combin[3] = { cycles[0] * cycles[1], cycles[0] * cycles[2], cycles[1] * cycles[2] };
    return num / gcd(combin);
}

Point3* parse_input(FILE* input) {
    Point3* moons = malloc(NB_MOONS * sizeof(Point3));
    char buffer[LINE_MAX] = { 0 };
    int i = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        strtok(buffer, "=");
        int x = atoi(strtok(NULL, "="));
        int y = atoi(strtok(NULL, "="));
        int z = atoi(strtok(NULL, "="));
        moons[i++] = (Point3){ .x = x, .y = y, .z = z };
    }
    return moons;
}

int main() {
    FILE* input = fopen("data/day_12.txt", "r");
    Point3* moons_pos_init = parse_input(input);
    Point3* moons_vel_init = calloc(NB_MOONS, sizeof(Point3));
    fclose(input);

    Point3* moons_pos_1000 = malloc(NB_MOONS * sizeof(Point3));
    Point3* moons_vel_1000 = malloc(NB_MOONS * sizeof(Point3));

    Point3* moons_pos = malloc(NB_MOONS * sizeof(Point3));
    memcpy(moons_pos, moons_pos_init, NB_MOONS * sizeof(Point3));
    Point3* moons_vel = calloc(NB_MOONS, sizeof(Point3));

    uint64_t cycles[3];
    for (int axis = 0; axis < 3; ++axis) {
        int k = 0;
        while (k == 0 || !is_cycle(moons_pos, moons_pos_init, moons_vel, moons_vel_init)) {
            simulate_axis(moons_pos, moons_vel, axis);
            ++k;
            if (k == 1000) {
                for (int i = 0; i < NB_MOONS; ++i) {
                    moons_pos_1000[i].components[axis] = moons_pos[i].components[axis];
                    moons_vel_1000[i].components[axis] = moons_vel[i].components[axis];
                }
            }
        }
        cycles[axis] = k;
    }
    int energy_1000 = 0;
    for (int i = 0; i < NB_MOONS; ++i) {
        int pot = abs(moons_pos_1000[i].x) + abs(moons_pos_1000[i].y) + abs(moons_pos_1000[i].z);
        int kin = abs(moons_vel_1000[i].x) + abs(moons_vel_1000[i].y) + abs(moons_vel_1000[i].z);
        energy_1000 += pot * kin;
    }

    free(moons_pos_1000);
    free(moons_vel_1000);
    free(moons_pos);
    free(moons_vel);
    free(moons_pos_init);
    free(moons_vel_init);

    printf("Part 1: %d\n", energy_1000);
    printf("Part 2: %lu\n", lcm(cycles));
}