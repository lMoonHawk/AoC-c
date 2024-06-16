#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int id;
    uint8_t counter[60];
} Guard;

int timestamp_cmp(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

size_t indexof(int guard_id, Guard* guards) {
    for (size_t i = 0; i < da_length(guards); i++)
        if (guards[i].id == guard_id)
            return i;
    return da_length(guards);
}

Guard* parse_input(FILE* input) {
    char** timestamps = da_create(sizeof(*timestamps));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r')
        da_append(timestamps, (char*) { strdup(buffer) });
    qsort(timestamps, da_length(timestamps), sizeof(*timestamps), timestamp_cmp);

    Guard* guards = da_create(sizeof(*guards));

    uint8_t prev_minute = 0;
    size_t guard_idx = 0;
    for (size_t i = 0; i < da_length(timestamps); i++) {
        strtok(timestamps[i], ":");
        int minute = atoi(strtok(NULL, " "));
        char* descr = strtok(NULL, "#");
        // Guard
        if (descr[0] == 'G') {
            int guard_id = atoi(strtok(NULL, " "));
            guard_idx = indexof(guard_id, guards);
            if (guard_idx == da_length(guards))
                da_append(guards, ((Guard) {.id = guard_id, .counter = { 0 } }));
            // falls asleep
        } else if (descr[0] == 'f') {
            prev_minute = minute;
            // wakes up
        } else if (descr[0] == 'w') {
            for (int t = prev_minute; t < minute; ++t)
                ++guards[guard_idx].counter[t];
        }
        free(timestamps[i]);
    }
    da_free(timestamps);
    return guards;
}

int main() {
    FILE* input = fopen("data/day_04.txt", "r");
    Guard* guards = parse_input(input);
    fclose(input);

    int max_minutes_cnt = 0;
    int best_guard_max_minute = 0;
    int best_guard_id_p1 = 0;

    int max_minute_all_guards = 0;
    int max_minute_cnt = 0;
    int best_guard_id_p2 = 0;

    for (size_t i = 0; i < da_length(guards); i++) {
        int guard_minutes_cnt = 0;
        int guard_max_minute_cnt = 0;
        int guard_max_minute = 0;
        for (int t = 0; t < 60; ++t) {
            guard_minutes_cnt += guards[i].counter[t];
            if (guards[i].counter[t] > guard_max_minute_cnt) {
                guard_max_minute_cnt = guards[i].counter[t];
                guard_max_minute = t;
                if (guard_max_minute_cnt > max_minute_cnt) {
                    max_minute_cnt = guard_max_minute_cnt;
                    max_minute_all_guards = t;
                    best_guard_id_p2 = guards[i].id;
                }
            }
        }
        if (guard_minutes_cnt > max_minutes_cnt) {
            max_minutes_cnt = guard_minutes_cnt;
            best_guard_id_p1 = guards[i].id;
            best_guard_max_minute = guard_max_minute;
        }
    }
    printf("Part 1: %d\n", best_guard_id_p1 * best_guard_max_minute);
    printf("Part 2: %d\n", best_guard_id_p2 * max_minute_all_guards);
    da_free(guards);
}