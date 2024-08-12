#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 1024

void parse_input(FILE* input, int(**boards)[25], int** drawn) {
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    char* tok = strtok(buffer, ",");
    while (tok != NULL) {
        da_append(*drawn, (int) { atoi(tok) });
        tok = strtok(NULL, ",");
    }
    fgets(buffer, sizeof(buffer), input);

    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int board[25];
        for (int i = 0; i < 5; ++i) {
            tok = strtok(buffer, " ");
            for (int j = 0; j < 5; ++j) {
                board[i * 5 + j] = atoi(tok);
                tok = strtok(NULL, " ");
            }
            fgets(buffer, sizeof(buffer), input);
        }
        da_append(*boards, board);
    }
}

bool board_update(int board[25], int drawn) {
    for (int i = 0; i < 5; ++i) {
        bool col_win = true;
        bool row_win = true;
        for (int j = 0; j < 5; ++j) {
            if (board[i * 5 + j] == drawn)
                board[i * 5 + j] = -1;
            if (board[j * 5 + i] == drawn)
                board[j * 5 + i] = -1;
            if (board[i * 5 + j] >= 0)
                row_win = false;
            if (board[j * 5 + i] >= 0)
                col_win = false;
        }
        if (row_win || col_win)
            return true;
    }
    return false;
}

int calc_score(int board[25], int drawn) {
    int unmarked = 0;
    for (int i = 0; i < 25; ++i)
        if (board[i] >= 0)
            unmarked += board[i];
    return unmarked * drawn;
}

int main() {
    FILE* input = fopen("data/day_04.txt", "r");
    int* drawn = da_create(sizeof(*drawn));
    int(*boards)[25] = da_create(sizeof(*boards));
    parse_input(input, &boards, &drawn);
    fclose(input);

    int board_cnt = da_length(boards);
    int drawn_cnt = da_length(drawn);

    bool* won = calloc(board_cnt, sizeof(bool));
    int win_count = 0;
    int score_first = 0;
    int score_last = 0;
    for (int i = 0; i < drawn_cnt; ++i) {
        for (int j = 0; j < board_cnt; ++j) {
            if (won[j])
                continue;
            if (board_update(boards[j], drawn[i])) {
                won[j] = true;
                ++win_count;
                if (win_count == 1) {
                    score_first = calc_score(boards[j], drawn[i]);
                } else if (win_count == drawn_cnt) {
                    score_last = calc_score(boards[j], drawn[i]);
                    break;
                }
            }
        }
        if (win_count == drawn_cnt)
            break;
    }
    free(won);
    da_free(drawn);
    da_free(boards);

    printf("Part 1: %d\n", score_first);
    printf("Part 2: %d\n", score_last);
}