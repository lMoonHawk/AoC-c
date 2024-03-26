#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define WIRES_MAX 500

enum Operator {
    ASSIGN,
    UNARY,
    BINARY,
};

typedef struct {
    char wire[10];
    uint16_t value;
    int available;
    int type;
    char connection[3][10];
} Wire;

void parse_wires(int k, Wire table[WIRES_MAX], char* buffer) {
    char* token;
    table[k].available = 0;
    table[k].value = 0;
    buffer[strcspn(buffer, "\n")] = 0;

    token = strtok(buffer, " ");
    int i = 0;
    while (strcmp(token, "->") != 0) {
        strcpy(table[k].connection[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    table[k].type = i - 1;

    token = strtok(NULL, " ");
    strcpy(table[k].wire, token);
}

uint16_t eval(const char* wire, Wire table[WIRES_MAX]) {
    if (isdigit(wire[0])) return atoi(wire);

    for (size_t i = 0; i < WIRES_MAX; i++) {
        if (strcmp(wire, table[i].wire) != 0) continue;
        if (table[i].available == 1) return table[i].value;
        char(*con)[10] = table[i].connection;

        switch (table[i].type) {
            case ASSIGN:
                table[i].value = eval(con[0], table);
                break;

            case UNARY:
                if (strcmp(con[0], "NOT") == 0) {
                    table[i].value = ~eval(con[1], table);
                }
                else {
                    assert(1 && "Unreachable");
                }
                break;

            case BINARY:
                if (strcmp(con[1], "AND") == 0) {
                    table[i].value = eval(con[0], table) & eval(con[2], table);
                }
                else if (strcmp(con[1], "OR") == 0) {
                    table[i].value = eval(con[0], table) | eval(con[2], table);
                }
                else if (strcmp(con[1], "LSHIFT") == 0) {
                    table[i].value = eval(con[0], table) << eval(con[2], table);
                }
                else if (strcmp(con[1], "RSHIFT") == 0) {
                    table[i].value = eval(con[0], table) >> eval(con[2], table);
                }
                else {
                    assert(1 && "Unreachable");
                }
                break;

            default:
                assert(1 && "Unreachable");
        }
        table[i].available = 1;
        return table[i].value;
    }
    return -1;
}

int main() {
    Wire table[WIRES_MAX];
    FILE* input = fopen("data/day_07.txt", "r");

    size_t k = 0;
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), input) != NULL) parse_wires(k++, table, buffer);
    fclose(input);

    uint16_t wire_a = eval("a", table);

    for (size_t i = 0; i < WIRES_MAX; i++) {
        table[i].available = 0;
        if (strcmp(table[i].wire, "b") == 0) {
            table[i].available = 1;
            table[i].value = wire_a;
        }
    }
    uint16_t new_wire_a = eval("a", table);

    printf("Part 1: %d\n", wire_a);
    printf("Part 2: %d", new_wire_a);
    return 0;
}

