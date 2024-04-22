#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int speed;
    int time;
    int rest;
    int points;
} Reindeer;

void parse_input(FILE* input, Reindeer** reindeers) {
    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        strtok(buffer, " ");
        for (int i = 0; i < 2; ++i) strtok(NULL, " ");
        int speed = atoi(strtok(NULL, " "));
        for (int i = 0; i < 2; ++i) strtok(NULL, " ");
        int time = atoi(strtok(NULL, " "));
        for (int i = 0; i < 6; ++i) strtok(NULL, " ");
        int rest = atoi(strtok(NULL, " "));
        Reindeer entry = { speed, time, rest , 0 };
        da_append(*reindeers, entry);
    }
}

unsigned long best_distance(Reindeer* reindeers, unsigned long tot_time, unsigned long* dists) {
    unsigned long max_distance = 0;
    unsigned long distance;
    for (size_t i = 0; i < da_length(reindeers); ++i) {
        int s = reindeers[i].speed;
        int t = reindeers[i].time;
        int r = reindeers[i].rest;

        div_t cycle = div(tot_time, t + r);
        distance = s * t * cycle.quot + s * (t < cycle.rem ? t : cycle.rem);
        dists[i] = distance;
        if (distance > max_distance) max_distance = distance;
    }
    return max_distance;
}

unsigned long credit_lead(Reindeer* reindeers, unsigned long* dists, unsigned long max_dist) {
    unsigned long max_points = 0;
    for (size_t i = 0; i < da_length(reindeers); ++i) {
        if (dists[i] == max_dist) ++reindeers[i].points;
        if ((unsigned long)reindeers[i].points > max_points) max_points = reindeers[i].points;
    }
    return max_points;
}

int main() {
    const unsigned long tot_time_max = 2503;

    FILE* input = fopen("data/day_14.txt", "r");
    Reindeer* reindeers = da_create(sizeof(Reindeer));
    parse_input(input, &reindeers);

    size_t n = da_length(reindeers);
    unsigned long dists[n];

    unsigned long max_dist;
    unsigned long max_points;
    for (unsigned long tot_time = 1; tot_time <= tot_time_max; ++tot_time) {
        max_dist = best_distance(reindeers, tot_time, dists);
        max_points = credit_lead(reindeers, dists, max_dist);
    }
    printf("Part 1: %lu\n", max_dist);
    printf("Part 2: %lu", max_points);

    da_free(reindeers);
    fclose(input);
}