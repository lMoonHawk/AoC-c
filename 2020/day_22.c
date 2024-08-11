#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define LINE_MAX 256

void parse_input(FILE* input, Queue players[2]) {
    int card = 0;
    char buffer[LINE_MAX] = { 0 };
    for (size_t i = 0; i < 2; ++i) {
        players[i] = q_create(sizeof(int));
        fgets(buffer, sizeof(buffer), input);
        while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
            sscanf(buffer, "%d", &card);
            q_push(players[i], card);
        }
    }
}

void capture_state(Queue players[2], uint8_t state[100]) {
    for (int p = 0; p < 2; ++p) {
        int n = q_length(players[p]);
        int index = players[p]->head;
        for (int i = 0; i < n; ++i) {
            state[i + p * 50] = ((int*)(players[p]->arr))[index];
            index = (index + 1) % da_capacity(players[p]->arr);
        }
        for (int i = n; i < 50; ++i)
            state[i + p * 50] = 0;
    }
}

int compute_score(Queue players[2], int winner) {
    int score = 0;
    if (q_length(players[1 - winner]) == 0) {
        int card;
        while (q_pop(players[winner], &card))
            score += card * (q_length(players[winner]) + 1);
    }
    return score;
}

int run_game(Queue players[2], int* winner, bool recursive) {
    Hset* prev_states = NULL;
    uint8_t state[100] = { 0 };
    if (recursive)
        prev_states = hs_create(2 * 50 * sizeof(uint8_t));

    while (q_length(players[0]) && q_length(players[1])) {
        if (recursive) {
            capture_state(players, state);
            if (hs_contains(*prev_states, state)) {
                *winner = 0;
                hs_free(prev_states);
                return compute_score(players, *winner);
            }
            hs_insert(prev_states, state);
        }

        int card[2];
        q_pop(players[0], &card[0]);
        q_pop(players[1], &card[1]);

        bool subgame = false;
        if (recursive && card[0] <= (int)q_length(players[0]) && card[1] <= (int)q_length(players[1])) {
            subgame = true;
            Queue next_players[2];
            for (size_t i = 0; i < 2; ++i)
                next_players[i] = q_slice(players[i], 0, card[i]);
            run_game(next_players, winner, recursive);
            for (size_t i = 0; i < 2; ++i)
                q_free(next_players[i]);
        }

        if ((subgame && *winner == 0) || (!subgame && card[0] > card[1])) {
            *winner = 0;
            q_push(players[0], card[0]);
            q_push(players[0], card[1]);
        } else {
            *winner = 1;
            q_push(players[1], card[1]);
            q_push(players[1], card[0]);
        }
    }
    if (recursive)
        hs_free(prev_states);
    return compute_score(players, *winner);
}

int main() {
    FILE* input = fopen("data/day_22.txt", "r");
    Queue players_init[2];
    parse_input(input, players_init);
    fclose(input);

    Queue players[2];
    for (size_t i = 0; i < 2; ++i)
        players[i] = q_copy(players_init[i]);

    int winner = 0;
    int score = run_game(players, &winner, false);
    int score_rec = run_game(players_init, &winner, true);

    for (size_t i = 0; i < 2; ++i) {
        q_free(players[i]);
        q_free(players_init[i]);
    }

    printf("Part 1: %d\n", score);
    printf("Part 2: %d\n", score_rec);
}