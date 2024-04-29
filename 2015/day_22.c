#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"

#define LINE_MAX 256

typedef enum {
    SPELL_MISSILE,
    SPELL_DRAIN,
    SPELL_SHIELD,
    SPELL_POISON,
    SPELL_RECHARGE,
} Spell_Type;

typedef enum {
    EFFECT_NONE,
    EFFECT_SHIELD,
    EFFECT_POISON,
    EFFECT_RECHARGE,
} Effect_Type;

typedef struct {
    Spell_Type type;
    int cost, damage, regen, base_timer;
    Effect_Type effect_type;
} Spell;

typedef struct {
    Effect_Type type;
    int timer;
} Effect;

typedef struct {
    int spent, hp, mana, armor, boss_hp;
    Effect effects[3];
} State;

const Spell spells[] = {
    (Spell) {.type = SPELL_MISSILE , .cost = 53 , .damage = 4, .regen = 0, .base_timer = 0, .effect_type = EFFECT_NONE},
    (Spell) {.type = SPELL_DRAIN   , .cost = 73 , .damage = 2, .regen = 2, .base_timer = 0, .effect_type = EFFECT_NONE},
    (Spell) {.type = SPELL_SHIELD  , .cost = 113, .damage = 0, .regen = 0, .base_timer = 6, .effect_type = EFFECT_SHIELD},
    (Spell) {.type = SPELL_POISON  , .cost = 173, .damage = 0, .regen = 0, .base_timer = 6, .effect_type = EFFECT_POISON},
    (Spell) {.type = SPELL_RECHARGE, .cost = 229, .damage = 0, .regen = 0, .base_timer = 5, .effect_type = EFFECT_RECHARGE},
};


int cmpstate(const void* s1, const void* s2) {
    // Priority: min spent, min boss_hp, max hp
    State state1 = *(State*)s1;
    State state2 = *(State*)s2;
    if (state1.spent == state2.spent) {
        if (state1.boss_hp == state2.boss_hp) {
            if (state1.hp == state2.hp) {
                return 0;
            }
            return state1.hp - state2.hp;
        }
        return state2.boss_hp - state1.boss_hp;
    }
    return state2.spent - state1.spent;
}

// Trigger effect and returns if the boss is killed
bool trigger_effect(State* s) {
    for (int i = 0; i < 3; ++i) {
        switch (s->effects[i].type) {
            case EFFECT_NONE:
                break;
            case EFFECT_SHIELD:
                s->armor = (s->effects[i].timer > 1) ? 7 : 0;
                break;
            case EFFECT_POISON:
                s->boss_hp -= 3;
                break;
            case EFFECT_RECHARGE:
                s->mana += 101;
                break;
        }
        s->effects[i].timer -= 1;
        if (s->effects[i].timer == 0) {
            s->effects[i].type = EFFECT_NONE;
        }
    }
    return s->boss_hp <= 0;
}

bool can_cast(State* s, Spell_Type i) {
    // Not enough mana for this spell
    if (spells[i].cost > s->mana) return false;

    // Spell with an effect already in play
    if (spells[i].effect_type != EFFECT_NONE) {
        for (int e = 0; e < 3; ++e) {
            if (spells[i].effect_type == s->effects[e].type) {
                return false;
            }
        }
    }
    return true;
}

// Cast spell and returns if the boss is killed
bool cast(State* s, Spell_Type st) {
    s->boss_hp -= spells[st].damage;
    s->hp += spells[st].regen;
    s->mana -= spells[st].cost;
    s->spent += spells[st].cost;
    if (spells[st].effect_type != EFFECT_NONE) {
        // Find the first open slot for the new effect
        for (int i = 0; i < 3; ++i) {
            if (s->effects[i].type == EFFECT_NONE) {
                s->effects[i] = (Effect){ spells[st].effect_type, spells[st].base_timer };
                break;
            }
        }
    }
    return s->boss_hp <= 0;
}

// Use priotity queue with priority given by cmpstate to search for the optimal state
int find_min_spent(int boss_hp, int boss_dmg, bool hard) {
    Pq states = pq_create(sizeof(State), cmpstate);
    {
        State init = { 0, 50, 500, 0, boss_hp, {{0}, {0}, {0}} };
        pq_push(states, init);
    }
    while (pq_length(states)) {
        State current;
        pq_pop(states, &current);

        if (hard) {
            current.hp -= 1;
            if (current.hp <= 0) continue;
        }

        // Effects - Boss can be killed here
        if (trigger_effect(&current))
            return current.spent;

        // Iterate through possible spells
        for (Spell_Type i = 0; i < 5; ++i) {
            if (!can_cast(&current, i))
                continue;
            State new = current;
            // Spell - Boss can be killed here
            if (cast(&new, i))
                return new.spent;
            // Effects - Boss can be killed here
            if (trigger_effect(&new))
                return new.spent;
            new.hp -= (boss_dmg - new.armor > 1) ? boss_dmg - new.armor : 1;
            // Receiving damage - Player can be killed here
            if (new.hp <= 0)
                continue;

            pq_push(states, new);
        }
    }
    return -1;
}

void parse_input(FILE* input, int* boss_hp, int* boss_dmg) {
    char buffer[LINE_MAX];

    fgets(buffer, sizeof(buffer), input);
    strtok(buffer, ":");
    *boss_hp = atoi(strtok(NULL, " "));

    fgets(buffer, sizeof(buffer), input);
    strtok(buffer, ":");
    *boss_dmg = atoi(strtok(NULL, " "));

}

int main() {
    FILE* input = fopen("data/day_22.txt", "r");

    int boss_hp, boss_dmg;
    parse_input(input, &boss_hp, &boss_dmg);

    printf("Part 1: %d\n", find_min_spent(boss_hp, boss_dmg, false));
    printf("Part 2: %d",   find_min_spent(boss_hp, boss_dmg, true ));
}