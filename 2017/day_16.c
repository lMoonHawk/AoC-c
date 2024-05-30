#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 65536

typedef enum {
    SPIN,
    EXCHANGE,
    PARTNER,
    MOVE_CNT,
} Move;

typedef struct {
    Move move;
    char args[2];
} Instruction;

typedef void (*dance_move) (char[2], char[17]);

void spin(char args[2], char* pgs) {
    int spin_amt = args[0];
    char tmp[spin_amt];
    memcpy(tmp, pgs + 16 - spin_amt, spin_amt);
    memmove(pgs + spin_amt, pgs, 16 - spin_amt);
    memcpy(pgs, tmp, spin_amt);
}

void exchange(char args[2], char* pgs) {
    int a0 = args[0];
    int a1 = args[1];

    char tmp = pgs[a0];
    pgs[a0] = pgs[a1];
    pgs[a1] = tmp;
}

void partner(char args[2], char* pgs) {
    char p1 = args[0];
    char p2 = args[1];
    int i1 = -1;
    char tmp = 0;
    for (int i = 0; i < 16; ++i) {
        if (pgs[i] == p1 || pgs[i] == p2) {
            if (i1 != -1) {
                pgs[i1] = pgs[i];
                pgs[i] = tmp;
                return;
            } else {
                tmp = pgs[i];
                i1 = i;
            }
        }
    }
    assert(0 && "unreachable");
}

Instruction* parse_input(FILE* input) {
    Instruction* dance = da_create(sizeof(*dance));
    Instruction buffer = { 0 };
    int argi = 0;
    bool new = true;

    char c = fgetc(input);
    while (c != EOF && c != '\r' && c != '\n') {
        if (new) {
            if (c == 's')      buffer.move = SPIN;
            else if (c == 'x') buffer.move = EXCHANGE;
            else if (c == 'p') buffer.move = PARTNER;
            new = false;
        } else if (c == ',') {
            da_append(dance, buffer);
            argi = 0;
            buffer.args[0] = 0;
            buffer.args[1] = 0;
            new = true;
        } else if (c == '/') {
            ++argi;
        } else {
            buffer.args[argi] = buffer.args[argi] * 10 + c;
            if (buffer.move != PARTNER)
                buffer.args[argi] -= '0';
        }
        c = fgetc(input);
    }
    da_append(dance, buffer);
    return dance;
}

void perform_dance(char* pgs, Instruction* dance, int rounds) {
    dance_move moves[MOVE_CNT] = { spin, exchange, partner };

    char(*previous)[17] = da_create(sizeof(*previous));
    da_append(previous, pgs[0]);

    for (int r = 0; r < rounds; ++r) {
        for (size_t i = 0; i < da_length(dance); ++i) {
            moves[dance[i].move](dance[i].args, pgs);
        }
        if (strcmp(pgs, previous[0]) == 0) {
            strncpy(pgs, (char*)&previous[(rounds - r - 1) % (r + 1)], 17);
            break;
        }
        da_append(previous, pgs[0]);
    }
    da_free(previous);
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    Instruction* dance = parse_input(input);
    fclose(input);

    char pgs[17] = { 0 };
    for (int i = 0; i < 16; ++i)
        pgs[i] = 'a' + i;

    char pgs_one_round[17];
    perform_dance(pgs, dance, 1);
    strncpy(pgs_one_round, pgs, 17);
    perform_dance(pgs, dance, 999999999);
    da_free(dance);

    printf("Part 1: %s\n", pgs_one_round);
    printf("Part 2: %s\n", pgs);
}