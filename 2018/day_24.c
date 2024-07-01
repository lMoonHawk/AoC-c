#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct Group {
    bool team;
    int init_size, size, hp, attack, initiative;
    char* attack_type, * weaknesses, * immunities;
    struct Group* target;
    bool targeted;
} Group;

int get_damage(Group* attacker, Group* target) {
    if (target->immunities != NULL && strstr(target->immunities, attacker->attack_type) != NULL)
        return 0;
    int attack = attacker->attack * attacker->size;
    if (target->weaknesses != NULL && strstr(target->weaknesses, attacker->attack_type) != NULL)
        attack *= 2;
    return attack;
}

int selection_priority_cmp(const void* a, const void* b) {
    Group* ga = *(Group**)a;
    Group* gb = *(Group**)b;
    if (ga->attack * ga->size == gb->attack * gb->size)
        return gb->initiative - ga->initiative;
    return gb->attack * gb->size - ga->attack * ga->size;
}

int target_cmp(Group* attacker, Group* a, Group* b) {
    int damage_to_b = get_damage(attacker, b);
    if (a == NULL) {
        if (damage_to_b == 0)
            return 0;
        return 1;
    }
    int damage_to_a = get_damage(attacker, a);
    if (damage_to_a == damage_to_b) {
        if (a->attack * a->size == b->attack * b->size) {
            return b->initiative - a->initiative;
        }
        return b->attack * b->size - a->attack * a->size;
    }
    return damage_to_b - damage_to_a;
}

int attack_prority_cmp(const void* a, const void* b) {
    return (*(Group**)b)->initiative - (*(Group**)a)->initiative;
}

bool run_combat(Group* groups, int* score) {
    size_t n = da_length(groups);
    Group* pgroups[n];
    for (size_t i = 0; i < n; ++i)
        pgroups[i] = &groups[i];

    int team_sizes[2] = { 1, 1 };
    bool stop = false;
    while (!stop) {
        stop = true;
        memset(team_sizes, 0, sizeof(int) * 2);

        // SELECTION PHASE
        qsort(pgroups, n, sizeof(Group*), selection_priority_cmp);
        for (size_t i = 0; i < n; ++i) {
            team_sizes[pgroups[i]->team] += pgroups[i]->size;
            if (pgroups[i]->size <= 0)
                continue;

            pgroups[i]->target = NULL;
            for (size_t j = 0; j < n; ++j) {
                if (groups[j].size <= 0 || groups[j].targeted || groups[j].team == pgroups[i]->team)
                    continue;
                if (target_cmp(pgroups[i], pgroups[i]->target, &groups[j]) > 0)
                    pgroups[i]->target = &groups[j];
            }
            if (pgroups[i]->target != NULL)
                pgroups[i]->target->targeted = true;
        }

        // ATTACK PHASE
        qsort(pgroups, n, sizeof(Group*), attack_prority_cmp);
        for (size_t i = 0; i < n; ++i) {
            if (pgroups[i]->size <= 0 || pgroups[i]->target == NULL)
                continue;
            Group* target = pgroups[i]->target;
            int damage = get_damage(pgroups[i], target);
            int unit_lost = damage / target->hp;
            if (unit_lost > 0)
                stop = false;
            target->size -= unit_lost;
            pgroups[i]->target = NULL;
            target->targeted = false;
            if (target->size <= 0) {
                target->size = 0;
                if (target->target != NULL)
                    target->target->targeted = false;
            }
        }
    }
    if (team_sizes[0] == 0) {
        *score = team_sizes[1];
        return true;
    } else if (team_sizes[1] == 0) {
        *score = team_sizes[0];
        return false;
    }
    // Stalemate
    *score = 0;
    return false;
}

void parse_group(Group** groups, char* line, bool team) {
    Group group = { 0 };
    group.team = team;
    sscanf(line, "%d units each with %d hit points", &group.init_size, &group.hp);
    group.size = group.init_size;
    char* atck_str = strstr(line, "attack");
    char attack_type_buffer[LINE_MAX] = { 0 };
    sscanf(atck_str, "attack that does %d %s damage at initiative %d", &group.attack, attack_type_buffer, &group.initiative);
    group.attack_type = strdup(attack_type_buffer);
    char* modifiers_start = strchr(line, '(');
    if (modifiers_start) {
        ++modifiers_start;
        char* weakness_start = "weak to ";
        char* weaknesses = strstr(modifiers_start, weakness_start);
        if (weaknesses) {
            weaknesses += strlen(weakness_start);
            size_t index_end = strcspn(weaknesses, ");");
            group.weaknesses = strndup(weaknesses, index_end);
        }
        char* immune_start = "immune to ";
        char* immunities = strstr(modifiers_start, immune_start);
        if (immunities) {
            immunities += strlen(immune_start);
            immunities[strcspn(immunities, ");")] = '\0';
            group.immunities = strdup(immunities);
        }
    }
    da_append(*groups, group);
}

Group* parse_input(FILE* input) {
    Group* groups = da_create(sizeof(*groups));
    char buffer[LINE_MAX] = { 0 };
    for (int i = 1; i >= 0; --i) {
        fgets(buffer, sizeof(buffer), input);
        while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r')
            parse_group(&groups, buffer, i);
    }
    return groups;
}

int main() {
    FILE* input = fopen("data/day_24.txt", "r");
    Group* groups = parse_input(input);
    fclose(input);

    int score;
    run_combat(groups, &score);

    int score_boost;
    while (!run_combat(groups, &score_boost)) {
        for (size_t i = 0; i < da_length(groups); ++i) {
            groups[i].size = groups[i].init_size;
            if (groups[i].team)
                ++groups[i].attack;
        }
    }

    for (size_t i = 0; i < da_length(groups); ++i) {
        free(groups[i].weaknesses);
        free(groups[i].immunities);
        free(groups[i].attack_type);
    }
    da_free(groups);

    printf("Part 1: %d\n", score);
    printf("Part 2: %d\n", score_boost);
}