#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define _LINE_MAX 256

typedef enum {
    OPEN,
    WALL,
    UNIT,
} Type;

typedef enum {
    ELF,
    GOBLIN,
} Team;

typedef struct {
    Point2 pos;
    int health, attack;
    Team team;
} Unit;

typedef struct {
    Type type;
    Unit* unit;
} Cell;

typedef struct {
    Point2 pos;
    Point2 first_step;
    int steps;
    Unit* target;
} State;

Point2 directions[4] = { { 0, -1 }, { -1, 0 }, { 1, 0 }, { 0, 1 } };

int p2read_order(const void* a, const void* b) {
    Point2 p1 = *(Point2*)a;
    Point2 p2 = *(Point2*)b;
    if (p1.y == p2.y)
        return p1.x - p2.x;
    return p1.y - p2.y;
}

int unitcmp(const void* a, const void* b) {
    Unit* ua = *(Unit**)a;
    Unit* ub = *(Unit**)b;
    return p2read_order(&ua->pos, &ub->pos);
}

Unit* get_target_in_reach(Unit* unit, size_t stride, Cell(*map)[stride]) {
    Unit* target = NULL;
    for (size_t i = 0; i < 4; ++i) {
        Point2 pos = p2add(unit->pos, directions[i]);
        if (map[pos.y][pos.x].type == UNIT) {
            Unit* candidate = map[pos.y][pos.x].unit;
            if (candidate->team != unit->team && candidate->health > 0)
                if (target == NULL || candidate->health < target->health)
                    target = candidate;
        }
    }
    return target;
}

State* best_candidate(State* candidates) {
    State* best = NULL;
    for (size_t i = 0; i < da_length(candidates); ++i) {
        State* candidate = &candidates[i];
        if (best == NULL) {
            best = candidate;
        } else {
            int pos_order = p2read_order(&candidate->pos, &best->pos);
            if (pos_order < 0 || (pos_order == 0 && p2read_order(&candidate->first_step, &best->first_step) < 0)) {
                best = candidate;
            }
        }
    }
    return best;
}

Unit* get_target_after_move(Unit* unit, size_t stride, Cell(*map)[stride]) {
    Queue queue = q_create(sizeof(State));
    q_push(queue, ((State){.pos = unit->pos, .first_step = { 0 }, .steps = 0, .target = NULL }));
    Hset* visited = hs_create(sizeof(Point2));

    int min_steps = INT_MAX;
    State* candidates = da_create(sizeof(*candidates));
    while (q_length(queue)) {
        State current;
        q_pop(queue, &current);

        // Goal
        if (current.steps > min_steps - 1)
            break;

        for (size_t i = 0; i < 4; ++i) {
            Point2 next_pos = p2add(current.pos, directions[i]);

            if (hs_contains(*visited, &next_pos))
                continue;
            hs_insert(visited, &next_pos);

            Point2 first_step = current.first_step;
            if (first_step.x == 0 && first_step.y == 0)
                first_step = next_pos;

            if (map[next_pos.y][next_pos.x].type == WALL)
                continue;

            if (map[next_pos.y][next_pos.x].type == UNIT) {
                Unit* other = map[next_pos.y][next_pos.x].unit;
                if (other->health > 0) {
                    if (other->team != unit->team) {
                        da_append(candidates, ((State){.pos = current.pos, .first_step = first_step, .steps = current.steps + 1, .target = other }));
                        if (min_steps == INT_MAX)
                            min_steps = current.steps + 1;
                    }
                    continue;
                }
            }
            q_push(queue, ((State){.pos = next_pos, .first_step = first_step, .steps = current.steps + 1, .target = NULL }));
        }
    }
    State* state_target = best_candidate(candidates);
    hs_free(visited);
    q_free(queue);

    if (state_target != NULL) {
        // Move the unit
        map[unit->pos.y][unit->pos.x].type = OPEN;
        unit->pos = state_target->first_step;
        map[unit->pos.y][unit->pos.x].type = UNIT;
        map[unit->pos.y][unit->pos.x].unit = unit;
        // In reach of the target
        if (state_target->steps == 2) {
            Unit* target = state_target->target;
            da_free(candidates);
            return target;
        }
    }
    da_free(candidates);
    return NULL;
}

bool check_team_win(size_t nb_units, Unit* punits[nb_units], Team team) {
    for (size_t i = 0; i < nb_units; ++i)
        if (punits[i]->team != team && punits[i]->health > 0)
            return false;
    return true;
}

int get_health(size_t nb_units, Unit* punits[nb_units]) {
    int score = 0;
    for (size_t i = 0; i < nb_units; ++i)
        score += punits[i]->health > 0 ? punits[i]->health : 0;
    return score;
}


bool get_combat_outcome(size_t nb_units, Unit* punits[nb_units], size_t stride, Cell(*map)[stride], bool no_elf_death, uint64_t* score) {
    int rounds = 0;
    while (1) {
        for (size_t i = 0; i < nb_units; ++i) {
            Unit* unit = punits[i];
            if (unit->health <= 0) {
                if (unit->team == ELF && no_elf_death) {
                    return false;
                }
                continue;
            }
            if (check_team_win(nb_units, punits, unit->team)) {
                *score = get_health(nb_units, punits) * rounds;
                return unit->team == ELF;
            }
            Unit* target = get_target_in_reach(unit, stride, map);
            if (target != NULL) {
                target->health -= unit->attack;
            } else if ((target = get_target_after_move(unit, stride, map)) != NULL) {
                target->health -= unit->attack;
            }
        }
        qsort(punits, nb_units, sizeof(Unit*), unitcmp);
        ++rounds;
    }
}


bool run_combat(Unit* units, size_t stride, Cell(*map)[stride], int elf_attack, bool no_elf_death, uint64_t* score) {
    // Create an array of pointers to units so we can sort without affecting the map which references units
    size_t nb_units = da_length(units);
    Unit* units_cpy = da_copy(units);
    Unit** punits = malloc(nb_units * sizeof(Unit*));
    Cell(*map_cpy)[stride] = da_copy(map);
    for (size_t i = 0; i < nb_units; ++i) {
        punits[i] = &units_cpy[i];
        map_cpy[punits[i]->pos.y][punits[i]->pos.x].unit = punits[i];
        if (punits[i]->team == ELF)
            punits[i]->attack = elf_attack;
    }

    bool outcome = get_combat_outcome(nb_units, punits, stride, map_cpy, no_elf_death, score);
    da_free(units_cpy);
    da_free(map_cpy);
    free(punits);
    return outcome;
}

void* parse_input(FILE* input, Unit** units, size_t* stride) {
    Cell* map = da_create(sizeof(*map));
    *units = da_create(sizeof(Unit));

    char buffer[_LINE_MAX] = { 0 };
    int y = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int x = 0;
        while (buffer[x] != '\n' && buffer[x] != '\r') {
            switch (buffer[x]) {
                case '.':
                    da_append(map, ((Cell){.type = OPEN, .unit = NULL }));
                    break;
                case '#':
                    da_append(map, ((Cell){.type = WALL, .unit = NULL }));
                    break;
                case 'E':
                    da_append(map, ((Cell){.type = UNIT, .unit = NULL }));
                    da_append(*units, ((Unit) {.pos = { x, y }, .health = 200, .attack = 3, .team = ELF }));
                    break;
                case 'G':
                    da_append(map, ((Cell){.type = UNIT, .unit = NULL }));
                    da_append(*units, ((Unit) {.pos = { x, y }, .health = 200, .attack = 3, .team = GOBLIN }));
                    break;
            }
            ++x;
        }
        *stride = x;
        ++y;
    }
    return map;
}

int main() {
    FILE* input = fopen("data/day_15.txt", "r");

    Unit* units = NULL;
    size_t stride;
    void* map = parse_input(input, &units, &stride);
    fclose(input);

    int elf_attack = 3;

    uint64_t outcome_p1 = 0;
    run_combat(units, stride, map, elf_attack++, false, &outcome_p1);

    uint64_t outcome_p2 = 0;
    while (!run_combat(units, stride, map, elf_attack++, true, &outcome_p2));

    da_free(map);
    da_free(units);

    printf("Part 1: %lu\n", outcome_p1);
    printf("Part 2: %lu\n", outcome_p2);
}