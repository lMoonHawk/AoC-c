#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define LETTERS 26


int checksum(char** boxes) {
    int dup2 = 0, dup3 = 0;
    for (size_t i = 0; i < da_length(boxes); ++i) {
        int counter[LETTERS] = { 0 };
        bool has_dup2 = false;
        bool has_dup3 = false;
        char* s = boxes[i];
        while (*s != '\0')
            ++counter[*(s++) - 'a'];

        for (int l = 0; l < LETTERS; ++l) {
            if (counter[l] == 2) has_dup2 = true;
            if (counter[l] == 3) has_dup3 = true;
        }
        dup2 += has_dup2;
        dup3 += has_dup3;
    }
    return dup2 * dup3;
}

char* common_letters(char** boxes) {
    for (size_t i = 0; i < da_length(boxes) - 1; ++i) {
        for (size_t j = i + 1; j < da_length(boxes); ++j) {
            int diff = 0;
            int diff_index = 0;
            char* c1 = boxes[i];
            char* c2 = boxes[j];
            while (*c1 != '\0' && *c2 != '\0') {
                if (*(c1++) != *(c2++))
                    ++diff;
                if (diff > 1)
                    break;
                else if (diff == 0)
                    ++diff_index;
            }
            if (diff == 1) {
                char* out = strdup(boxes[i]);
                memmove(out + diff_index, out + diff_index + 1, strlen(out) - diff_index);
                return out;
            }
        }
    }
    return NULL;
}

char** parse_input(FILE* input) {
    char** boxes = da_create(sizeof(*boxes));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* s = buffer;
        while (*s != '\r' && *s != '\n') ++s;
        *s = '\0';
        da_append(boxes, (char*) { strdup(buffer) });
    }
    return boxes;
}

int main() {
    FILE* input = fopen("data/day_02.txt", "r");
    char** boxes = parse_input(input);
    fclose(input);

    char* common = common_letters(boxes);
    
    printf("Part 1: %d\n", checksum(boxes));
    printf("Part 2: %s\n", common);

    for (size_t i = 0; i < da_length(boxes); ++i)
        free(boxes[i]);
    da_free(boxes);
    free(common);
}
