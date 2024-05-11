#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINE_MAX 256
#define ROWS 6
#define COLS 50

typedef enum {
    RECT,
    ROTROW,
    ROTCOL,
} Opcode;

typedef struct {
    Opcode operation;
    int arg1, arg2;
} Instruction;

Instruction parse_instruction(char* line) {
    char* operation = strtok(line, " ");
    Opcode op;
    int arg1, arg2;
    if (strcmp(operation, "rect") == 0) {
        op = RECT;
        arg1 = atoi(strtok(NULL, "x"));
        arg2 = atoi(strtok(NULL, "\n"));
    }
    else {
        operation = strtok(NULL, " ");
        if (strcmp(operation, "row") == 0)
            op = ROTROW;
        else
            op = ROTCOL;
        strtok(NULL, "=");
        arg1 = atoi(strtok(NULL, " "));
        strtok(NULL, " ");
        arg2 = atoi(strtok(NULL, "\n"));
    }
    return (Instruction) { .operation = op, .arg1 = arg1, .arg2 = arg2 };
}

void draw_rect(bool screen[ROWS][COLS], int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            screen[i][j] = true;
}

void rot_row(bool screen[ROWS][COLS], int row, int shift) {
    shift = shift % COLS;
    bool temp[shift];
    for (int i = COLS - shift, j = 0; i < COLS; i++, j++)
        temp[j] = screen[row][i];
    for (int i = COLS - 1; i >= shift; i--)
        screen[row][i] = screen[row][i - shift];
    for (int i = 0; i < shift; i++)
        screen[row][i] = temp[i];
}

void rot_col(bool screen[ROWS][COLS], int col, int shift) {
    shift = shift % ROWS;
    bool temp[shift];
    for (int i = ROWS - shift, j = 0; i < ROWS; i++, j++)
        temp[j] = screen[i][col];
    for (int i = ROWS - 1; i >= shift; i--)
        screen[i][col] = screen[i - shift][col];
    for (int i = 0; i < shift; i++)
        screen[i][col] = temp[i];
}

void render_txt(bool screen[ROWS][COLS], char render[ROWS * (COLS + 1)]) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            render[i * (COLS + 1) + j] = screen[i][j] ? '#' : ' ';
        }
        render[i * (COLS + 1) + COLS] = '\n';
    }
    render[(ROWS - 1) * (COLS + 1) + COLS] = '\0';
}

int main() {
    FILE* input = fopen("data/day_08.txt", "r");

    bool screen[ROWS][COLS] = { 0 };
    void (*dispatch[3]) (bool[ROWS][COLS], int, int) = { draw_rect, rot_row, rot_col };

    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        Instruction in = parse_instruction(buffer);
        dispatch[in.operation](screen, in.arg1, in.arg2);
    }

    int lit_cnt = 0;
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            lit_cnt += screen[i][j];

    char render[ROWS * (COLS + 1)] = { 0 };
    render_txt(screen, render);

    printf("Part 1: %d\n", lit_cnt);
    printf("Part 2: \n%s\n", render);

    fclose(input);
}