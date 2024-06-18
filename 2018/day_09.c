#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 256

typedef struct Node {
    uint64_t value;
    struct Node* prev, * next;
} Node;

Node* marble_create(int value, Node* prev, Node* next) {
    Node* node = malloc(sizeof(Node));
    node->value = value;
    node->prev = prev == NULL ? node : prev;
    node->next = next == NULL ? node : next;
    return node;
}

uint64_t get_max_score(uint32_t players_count, uint64_t players_scores[players_count]) {
    uint64_t max_score = 0;
    for (uint64_t i = 0; i < players_count; ++i)
        if (players_scores[i] > max_score)
            max_score = players_scores[i];
    return max_score;
}

int main() {
    FILE* input = fopen("data/day_09.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    fclose(input);

    uint32_t players_count = atoi(strtok(buffer, " "));
    for (int i = 0; i < 5; ++i) strtok(NULL, " ");
    uint64_t last_marble = atoi(strtok(NULL, " "));

    uint64_t* players_scores = malloc(players_count * sizeof(*players_scores));
    memset(players_scores, 0, players_count * sizeof(*players_scores));

    Node* current = marble_create(0, NULL, NULL);
    uint64_t score_init = 0;
    for (uint64_t i = 1; i < last_marble * 100; i++) {
        if (i % 23 != 0) {
            Node* left = current->next;
            Node* right = left->next;
            Node* new_marble = marble_create(i, left, right);
            left->next = new_marble;
            right->prev = new_marble;
            current = new_marble;
        } else {
            uint32_t current_player = (i - 1) % players_count;

            Node* rem = current->prev;
            for (int n = 0; n < 6; ++n)
                rem = rem->prev;

            players_scores[current_player] += i + rem->value;
            Node* left = rem->prev;
            Node* right = rem->next;
            free(rem);
            left->next = right;
            right->prev = left;
            current = right;
        }
        if(i == last_marble)
            score_init = get_max_score(players_count, players_scores);
    }

    while (current->next != current) {
        Node* rem = current->next;
        current->next = current->next->next;
        free(rem);
    }
    free(current);

    printf("Part 1: %lu\n", score_init);
    printf("Part 2: %lu\n", get_max_score(players_count, players_scores));

    free(players_scores);
}