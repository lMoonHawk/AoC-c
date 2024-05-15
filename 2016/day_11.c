#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"


#define LINE_MAX 256
#define FLOORS_CNT 4
#define LAYOUT_CNT (FLOORS_CNT) * (FLOORS_CNT)
#define HS_CAPA 50000

typedef struct {
    uint8_t elevator;
    uint8_t layout[LAYOUT_CNT];
} State;

typedef struct {
    char* name;
    uint8_t floor_chip, floor_gen;
} Pair;

// Add the element if not found. Returns a pointer to the element in the array
Pair* get_or_create(Pair** pairs, const char* name) {
    size_t i;
    for (i = 0; i < da_length(*pairs); ++i) {
        if (strcmp(name, (*pairs)[i].name) == 0)
            return &((*pairs)[i]);
    }
    da_append(*pairs, ((Pair) {strdup(name), 0, 0}));
    return &((*pairs)[i]);
}

void parse_input(FILE* input, uint8_t layout[LAYOUT_CNT]) {
    uint8_t floor = 0;
    Pair* pairs = da_create(sizeof(Pair));

    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input)) {
        strtok(buffer, " ");
        for (int i = 0; i < 3; ++i) strtok(NULL, " ");
        if (strcmp(strtok(NULL, " "), "nothing") == 0)
            continue;

        char* name = strtok(NULL, " -");
        while (name != NULL) {
            Pair* current = get_or_create(&pairs, name);

            if (strcmp(strtok(NULL, " .,"), "generator") == 0) {
                current->floor_gen = floor;
            } else {
                current->floor_chip = floor;
                strtok(NULL, ".,");
            }
            strtok(NULL, " ");
            name = strtok(NULL, " -");
            if (name != NULL && strcmp(name, "a") == 0) {
                name = strtok(NULL, " -");
            }
        }
        ++floor;
    }

    for (size_t i = 0; i < da_length(pairs); ++i) {
        int index = 4 * pairs[i].floor_chip + pairs[i].floor_gen;
        ++layout[index];
        free(pairs[i].name);
    }
    da_free(pairs);
}

bool state_isempty(State s) {
    if (s.elevator != 0)
        return false;
    for (int i = 0; i < LAYOUT_CNT; ++i)
        if (s.layout[i] != 0)
            return false;
    return true;
}

bool state_isvalid(State s) {
    for (int i = 0; i < LAYOUT_CNT; ++i) {
        // Proceed if a pair exists where gen and chip are on different floors.
        if (i % 5 == 0 || s.layout[i] == 0)
            continue;
        // Iterate over potential pairs where gen floor = i.
        for (int j = 0; j < 4; j++) {
            if (s.layout[i / 4 + 4 * j] > 0)
                return false;
        }
    }
    return true;
}

bool state_isgoal(State s) {
    for (int i = 0; i < LAYOUT_CNT - 1; i++) {
        if (s.layout[i] > 0)
            return false;
    }
    return true;
}


size_t hash_state(State s) {
    int p = 31;
    size_t hash = 1;
    for (int i = 0; i < LAYOUT_CNT; i++) {
        hash *= p * (1 + s.layout[i] * 7);
        hash %= HS_CAPA;
    }
    hash *= p * (17 + s.elevator * 3);
    return hash % HS_CAPA;
}

void hs_add(State* hs, State v) {
    size_t index = hash_state(v);

    while (!state_isempty(hs[index])) {
        index = (index + 1 < HS_CAPA) ? index + 1 : 0;
    }
    hs[index] = v;
}

bool hs_in(State* hs, State v) {
    size_t index = hash_state(v);

    while (!state_isempty(hs[index])) {
        if (memcmp(&hs[index], &v, sizeof(State)) == 0)
            return true;
        index = (index + 1 < HS_CAPA) ? index + 1 : 0;
    }
    return false;
}

// Iterates through reachable states (without checking for single microchips yet).
// Start from state s and mutates state next
bool iter_moves(int i, State s, uint8_t floor, int next_floor, State* next) {
    int index_dec, index_inc;
    if (i < 4) {
        index_dec = 4 * floor + i;
        if (s.layout[index_dec] == 0)
            return false;
        index_inc = 4 * next_floor + i;
    } else {
        index_dec = 4 * (i - 4) + floor;
        if (index_dec >= 16) {
            printf("elevator = %d, index_dec = %d\n", floor, index_dec);
        }
        if (s.layout[index_dec] == 0)
            return false;
        index_inc = 4 * (i - 4) + next_floor;
    }
    --next->layout[index_dec];
    ++next->layout[index_inc];
    next->elevator = next_floor;
    return true;
}

int get_min_steps(State init) {
    State* seen = calloc(HS_CAPA, sizeof(State));

    Queue states = q_create(sizeof(State));
    q_push(states, init);

    int steps = 0;
    size_t space = 1;

    while (q_length(states)) {
        if (space == 0) {
            ++steps;
            space = q_length(states);
        }
        State current;
        q_pop(states, &current);
        if (state_isgoal(current)) {
            break;
        }
        int8_t moves[2] = { current.elevator - 1, current.elevator + 1 };
        for (int e = 0; e < 2; e += 1) {
            int8_t next_floor = moves[e];
            if (next_floor < 0 || next_floor >= FLOORS_CNT)
                continue;

            for (int i = 0; i < 8; i++) {
                State next = current;
                if (!iter_moves(i, current, current.elevator, next_floor, &next)) {
                    continue;
                }
                if (state_isvalid(next) && !hs_in(seen, next)) {
                    hs_add(seen, next);
                    q_push(states, next);
                }
                for (int j = 0; j < 8; j++) {
                    State next_2 = next;
                    if (!iter_moves(j, next, current.elevator, next_floor, &next_2)) {
                        continue;
                    }
                    if (state_isvalid(next_2) && !hs_in(seen, next_2)) {
                        hs_add(seen, next_2);
                        q_push(states, next_2);
                    }
                }
            }
        }
        --space;
    }

    free(seen);
    q_free(states);
    return steps;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");

    State init = { 0 };
    // States consists of the current floor of the elevator and the layout.
    // With 4 floor there are 4*4 = 16 possible types of pairs. We keep an array to count the number of pairs. 
    // Chip floor | Gen floor | Count
    //          0 |        0  | 2
    //          0 |        1  | 0 
    //                ...
    //          3 |        3  | 0
    // This makes the algorithm generic for the number of floors and also allowing for a large number of pairs.
    // Also, this makes the actual types of pairs interchangeable which lessens the search space.

    parse_input(input, init.layout);
    int min1 = get_min_steps(init);

    // Add 2 pairs on floor 0 for part 2.
    State init2 = init;
    init2.layout[0] += 2;
    int min2 = get_min_steps(init2);

    printf("Part 1: %d\n", min1);
    printf("Part 2: %d\n", min2);
    fclose(input);

}