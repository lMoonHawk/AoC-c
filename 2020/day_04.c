#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LINE_MAX 256
#define FIELD_MAX 32

typedef enum {
    BYR,
    IYR,
    EYR,
    HGT,
    HCL,
    ECL,
    PID,
    CID,
    FIELD_COUNT,
} Field;

Field str_to_field(char* label) {
    if (strcmp(label, "byr") == 0)
        return BYR;
    if (strcmp(label, "iyr") == 0)
        return IYR;
    if (strcmp(label, "eyr") == 0)
        return EYR;
    if (strcmp(label, "hgt") == 0)
        return HGT;
    if (strcmp(label, "hcl") == 0)
        return HCL;
    if (strcmp(label, "ecl") == 0)
        return ECL;
    if (strcmp(label, "pid") == 0)
        return PID;
    return CID;
}

bool is_field_valid(Field i, const char* field) {
    int field_int = atoi(field);
    int n = strlen(field);
    switch (i) {
        case BYR:
            return field_int >= 1920 && field_int <= 2002;
        case IYR:
            return field_int >= 2010 && field_int <= 2020;
        case EYR:
            return field_int >= 2020 && field_int <= 2030;
        case HGT:
            return (field[n - 2] == 'c' && field_int >= 150 && field_int <= 193) ||
                (field[n - 2] == 'i' && field_int >= 59 && field_int <= 76);
        case HCL:
            return (n == 7 && field[0] == '#' && strspn(field + 1, "0123456789abcdef") == 6);
        case ECL:
            return (n == 3) && (
                strcmp(field, "amb") == 0 ||
                strcmp(field, "blu") == 0 ||
                strcmp(field, "brn") == 0 ||
                strcmp(field, "gry") == 0 ||
                strcmp(field, "grn") == 0 ||
                strcmp(field, "hzl") == 0 ||
                strcmp(field, "oth") == 0);
        case PID:
            return (n == 9 && (strspn(field, "0123456789") == 9));
        case CID:
            return true;
        default:
            return false;
    }
    return false;
}

bool parse_next_passeport(FILE* input, char fields[FIELD_COUNT][FIELD_MAX]) {
    bool is_empty = true;
    char buffer[LINE_MAX] = { 0 };
    while (true) {
        if (fgets(buffer, sizeof(buffer), input) == NULL) {
            if (is_empty)
                return false;
            return true;
        }
        if (buffer[0] == '\n' || buffer[0] == '\r')
            return true;

        char* field = strtok(buffer, ":");
        char* value = strtok(NULL, " ");
        while (field != NULL) {
            is_empty = false;
            int n = strcspn(value, "\n\r");
            strncpy(fields[str_to_field(field)], value, n);
            field = strtok(NULL, ":");
            value = strtok(NULL, " ");
        }
    }
}


int main() {
    FILE* input = fopen("data/day_04.txt", "r");
    char fields[FIELD_COUNT][FIELD_MAX] = { 0 };

    int valid_p1 = 0;
    int valid_p2 = 0;
    while (parse_next_passeport(input, fields)) {
        bool is_valid_p1 = true;
        bool is_valid_p2 = true;
        for (Field i = 0; i < FIELD_COUNT; ++i) {
            if (i == CID)
                continue;
            if (fields[i][0] == '\0') {
                is_valid_p1 = false;
                break;
            }
            if (!is_field_valid(i, fields[i])) {
                is_valid_p2 = false;
            }
        }
        valid_p1 += is_valid_p1;
        valid_p2 += is_valid_p1 && is_valid_p2;
        memset(fields, 0, FIELD_MAX * FIELD_COUNT);
    }
    fclose(input);

    printf("Part 1: %d\n", valid_p1);
    printf("Part 2: %d\n", valid_p2);
}