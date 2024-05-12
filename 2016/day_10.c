#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"

#define LINE_MAX 256

typedef struct {
    int id;
    int held[2];
    int to[2];
    bool out[2];
} Bot;

int bot_cmp(const void* b1, const void* b2) { return ((Bot*)b1)->id - ((Bot*)b2)->id; }

typedef struct {
    int value, bot_id;
} Input_bin;


void parse_input(FILE* input, Bot** bots, Input_bin** values) {
    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        if (strcmp(strtok(buffer, " "), "bot") == 0) {
            int id = atoi(strtok(NULL, " "));
        
            bool out[2];
            int to[2];
            for (int i = 0; i < 2; ++i) {
                for (int s = 0; s < 3; ++s) strtok(NULL, " ");
                out[i] = strcmp(strtok(NULL, " "), "output") == 0;
                to[i] = atoi(strtok(NULL, " "));
            }
            da_append(*bots, ((Bot) {id, { -1, -1 }, {to[0], to[1]}, {out[0], out[1]}}));

        } else {
            int value = atoi(strtok(NULL, " "));
            for (int i = 0; i < 3; ++i) strtok(NULL, " ");
            int bot_id = atoi(strtok(NULL, " "));
            da_append(*values, ((Input_bin) {value, bot_id}));
        }
    }
}

// Gives chip to bot, keeping chips in ascending order.
// Returs the number of chips held.
int give_chip(Bot* bot, int chip) {
    assert(bot->held[1] == -1);
    if (bot->held[0] == -1) {
        bot->held[0] = chip;
        return 1;
    }
    bot->held[1] = chip;
    if (bot->held[0] > chip) {
        bot->held[1] = bot->held[0];
        bot->held[0] = chip;
    }
    return 2;
}

Queue setup_queue(Bot* bots, Input_bin* values) {
    Queue q = q_create(sizeof(Bot*));
    for (size_t i = 0; i < da_length(values); ++i) {
        int index = values[i].bot_id;
        Bot* bp = &bots[index];
        if (give_chip(bp, values[i].value) == 2) {
            q_push(q, bp);
        }
    }
    return q;
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");
    Bot* bots = da_create(sizeof(Bot));
    Input_bin* in_chips = da_create(sizeof(Input_bin));
    parse_input(input, &bots, &in_chips);
    fclose(input);

    // Order bots by id for fast access
    qsort(bots, da_length(bots), sizeof(Bot), bot_cmp);

    // Queue of pointers to bots
    Queue handling = setup_queue(bots, in_chips);
    da_free(in_chips);

    int bot_p1 = -1;
    int outputs = 1;
    while (q_length(handling)) {
        Bot* current;
        q_pop(handling, &current);

        if (bot_p1 == -1 && current->held[0] == 17 && current->held[1] == 61)
            bot_p1 = current->id;

        for (int i = 0; i < 2; ++i) {
            if (current->out[i] && current->to[i] <= 2) {
                outputs *= current->held[0];
            } else if (!current->out[i]) {
                Bot* receiver = &bots[current->to[i]];
                if (give_chip(receiver, current->held[i]) == 2) {
                    q_push(handling, receiver);
                }
            }
            current->held[i] = -1;
        }
    }
    printf("Part 1: %d\n", bot_p1);
    printf("Part 2: %d\n", outputs);

    da_free(bots);
    q_free(handling);
}