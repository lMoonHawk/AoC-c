#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define INTCODE_IMPLEMENTATION
#include "../common/intcode.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

Point2 p2adjacents[4] = {
    {.x = 0, .y = -1},
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0}
};

static inline int mod(int a, int b) {
    return (a % b + b) % b;
}

Hset* get_scaffolds(int64_t* program, Point2* robot_pos, int* facing) {
    IC_state* state = ic_create(program);
    ic_interpret(state);
    Hset* scaffolds = hs_create(sizeof(Point2));
    Point2 screen_pos = { 0 };
    char* facings = "^>v<";
    char* f;
    while (ic_output_length(state)) {
        char cell = ic_output_pop(state);
        if (cell == '#') {
            hs_insert(scaffolds, &screen_pos);
        } else if ((f = strchr(facings, cell)) != NULL) {
            *facing = f - facings;
            *robot_pos = screen_pos;
            hs_insert(scaffolds, &screen_pos);
        } else if (cell == '\n') {
            screen_pos.x = -1;
            ++screen_pos.y;
        }
        ++screen_pos.x;
    }
    ic_free(state);
    return scaffolds;
}

bool is_intersection(Hset* scaffolds, Point2* p) {
    for (int j = 0; j < 4; ++j) {
        Point2 neigh = p2add(*p, p2adjacents[j]);
        if (!hs_contains(*scaffolds, &neigh))
            return false;
    }
    return true;
}

int get_alignement(Hset* scaffolds) {
    int alignement = 0;
    for (size_t i = 0; i < scaffolds->capacity; ++i) {
        if (scaffolds->htable[i].key != NULL) {
            Point2* p = scaffolds->htable[i].key;
            if (is_intersection(scaffolds, p))
                alignement += p->x * p->y;
        }
    }
    return alignement;
}

int* get_instructions(Hset* scaffolds, Point2 robot_pos, int facing) {
    int* instructions = da_create(sizeof(*instructions));
    int n = 0;
    while (true) {
        Point2 front = p2add(robot_pos, p2adjacents[facing]);
        Point2 right = p2add(robot_pos, p2adjacents[mod(facing + 1, 4)]);
        Point2 left = p2add(robot_pos, p2adjacents[mod(facing - 1, 4)]);

        if (hs_contains(*scaffolds, &front)) {
            ++instructions[n - 1];
            robot_pos = front;
        } else if (hs_contains(*scaffolds, &right)) {
            facing = mod(facing + 1, 4);
            da_append(instructions, (int) { -1 });
            da_append(instructions, (int) { 1 });
            n += 2;
            robot_pos = right;
        } else if (hs_contains(*scaffolds, &left)) {
            facing = mod(facing - 1, 4);
            da_append(instructions, (int) { -2 });
            da_append(instructions, (int) { 1 });
            n += 2;
            robot_pos = left;
        } else {
            break;
        }
    }
    return instructions;
}

static inline bool eq_arr_subset(int f, int n, int p, int* instructions, int* functions[3], int lengths[3]) {
    return (n - p >= lengths[f] && memcmp(&instructions[p], functions[f], sizeof(int) * lengths[f]) == 0);
}

bool try_compress_instructions(int* instructions, char** routine, int* functions[3], int lengths[3]) {
    int n = da_length(instructions);
    bool B_found = false;
    bool C_found = false;

    char* _routine = da_create(sizeof(*_routine));
    for (int i = 0; i < 3; ++i)
        functions[i] = malloc(sizeof(int) * lengths[i]);
    int p = 0;

    // add A
    da_append(_routine, (char) { 'A' });
    memcpy(functions[0], &instructions[p], sizeof(int) * lengths[0]);
    p += lengths[0];

    while (true) {
        if (p == n) {
            *routine = _routine;
            return true;
        } else if (eq_arr_subset(0, n, p, instructions, functions, lengths)) {
            da_append(_routine, (char) { 'A' });
            p += lengths[0];
        } else if (!B_found) {
            if (n - p < lengths[1])
                break;
            B_found = true;
            memcpy(functions[1], &instructions[p], sizeof(int) * lengths[1]);
            da_append(_routine, (char) { 'B' });
            p += lengths[1];
        } else if (eq_arr_subset(1, n, p, instructions, functions, lengths)) {
            da_append(_routine, (char) { 'B' });
            p += lengths[1];
        } else if (!C_found) {
            if (n - p < lengths[2])
                break;
            C_found = true;
            memcpy(functions[2], &instructions[p], sizeof(int) * lengths[2]);
            da_append(_routine, (char) { 'C' });
            p += lengths[2];
        } else if (eq_arr_subset(2, n, p, instructions, functions, lengths)) {
            da_append(_routine, (char) { 'C' });
            p += lengths[2];
        } else {
            break;
        }
    }
    da_free(_routine);
    for (int i = 0; i < 3; ++i)
        free(functions[i]);
    return false;
}

void compress_instructions(int* instructions, char** routine, int* functions[3], int lengths[3]) {
    for (lengths[0] = 1; lengths[0] <= 10; ++lengths[0]) {
        for (lengths[1] = 1; lengths[1] <= 10; ++lengths[1]) {
            for (lengths[2] = 1; lengths[2] <= 10; ++lengths[2]) {
                if (try_compress_instructions(instructions, routine, functions, lengths)) {
                    return;
                }
            }
        }
    }
}

void ic_push_routine(IC_state* state, char* routine) {
    for (size_t i = 0; i < da_length(routine) - 1; ++i) {
        ic_input_push(state, routine[i]);
        ic_input_push(state, ',');
    }
    ic_input_push(state, routine[da_length(routine) - 1]);
    ic_input_push(state, '\n');
}

void ic_push_functions(IC_state* state, int* functions[3], int lengths[3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < lengths[i] - 1; ++j) {
            if (functions[i][j] == -1) {
                ic_input_push(state, 'R');
            } else if (functions[i][j] == -2) {
                ic_input_push(state, 'L');
            } else {
                char buffer[8] = { 0 };
                snprintf(buffer, 8, "%d", functions[i][j]);
                for (size_t k = 0; k < strlen(buffer); ++k) {
                    ic_input_push(state, buffer[k]);
                }
            }
            ic_input_push(state, ',');
        }
        char buffer[8] = { 0 };
        snprintf(buffer, 8, "%d", functions[i][lengths[i] - 1]);
        for (size_t k = 0; k < strlen(buffer); ++k) {
            ic_input_push(state, buffer[k]);
        }
        ic_input_push(state, '\n');
    }
}

int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    int64_t* program = ic_parse(input);
    fclose(input);

    Point2 robot_pos = { 0 };
    int facing = 0;
    Hset* scaffolds = get_scaffolds(program, &robot_pos, &facing);
    int intersections = get_alignement(scaffolds);
    int* instructions = get_instructions(scaffolds, robot_pos, facing);
    hs_free(scaffolds);

    char* routine = NULL;
    int* functions[3];
    int functions_len[3] = { 0 };
    compress_instructions(instructions, &routine, functions, functions_len);
    da_free(instructions);

    IC_state* state = ic_create(program);
    da_free(program);
    ic_override(state, 0, 2);

    ic_push_routine(state, routine);
    da_free(routine);
    ic_push_functions(state, functions, functions_len);
    for (int i = 0; i < 3; ++i)
        free(functions[i]);
    ic_input_push(state, 'n');
    ic_input_push(state, '\n');
    ic_interpret(state);
    int dust = 0;
    while (ic_output_length(state))
        dust = ic_output_pop(state);

    ic_free(state);

    printf("Part 1: %d\n", intersections);
    printf("Part 2: %d\n", dust);
}