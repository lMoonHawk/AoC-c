#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 256

void next_inc_value(char* value, int size) {
    int i = size - 1;
    while (i >= 0) {
        if (value[i] == '9') {
            value[i] = '0';
        } else {
            value[i]++;
            for (int k = i + 1; k < size; ++k)
                value[k] = value[k - 1];
            break;
        }
        --i;
    }
}

void count_consecutive(char* value, size_t size, int* atleast2, int* exactly2) {
    *atleast2 = 0;
    *exactly2 = 0;
    for (size_t i = 0; i < size - 1; ++i) {
        if (value[i] == value[i + 1]) {
            ++*atleast2;
            if ((i == 0 || value[i - 1] != value[i]) && (i + 2 == size || value[i + 1] != value[i + 2])) {
                ++*exactly2;
                return;
            }
        }
    }
}

int main() {
    FILE* input = fopen("data/day_04.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);
    char* bound_min = strtok(buffer, "-");
    char* bound_max = strtok(NULL, "\r\n");
    size_t min_size = strlen(bound_min);
    size_t max_size = strlen(bound_max);

    char* value = malloc(max_size + 1);
    memset(value, '0', max_size);
    strcpy(value + max_size - min_size, bound_min);
    for (size_t i = 0; i < max_size - 1; ++i)
        if (value[i + 1] < value[i])
            value[i + 1] = value[i];

    int valid_pwd_1 = 0;
    int valid_pwd_2 = 0;
    int atleast2, exactly2;
    while (strcmp(value, bound_max) < 0) {
        count_consecutive(value, max_size, &atleast2, &exactly2);
        if (atleast2 > 0) ++valid_pwd_1;
        if (exactly2 > 0) ++valid_pwd_2;
        next_inc_value(value, max_size);
    }
    free(value);

    printf("Part 1: %d\n", valid_pwd_1);
    printf("Part 2: %d\n", valid_pwd_2);
}