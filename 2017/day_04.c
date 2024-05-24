#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256
#define LETTERS 26

void get_words(char*** words, char* buffer) {
    char* word = strtok(buffer, " \r\n");
    while (word != NULL) {
        da_append(*words, word);
        word = strtok(NULL, " \r\n");
    }
}

int cntcmp(int* c1, int* c2) {
    for (int i = 0; i < LETTERS; ++i)
        if (c1[i] != c2[i])
            return c1[i] - c2[i];
    return 0;
}

void is_valid(size_t n, char** words, int(*counter)[LETTERS], bool* p1, bool* p2) {
    *p1 = true, * p2 = true;
    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (strcmp(words[i], words[j]) == 0) {
                *p1 = false, *p2 = false;
                return;
            }
            if (cntcmp(counter[i], counter[j]) == 0) {
                *p2 = false;
            }
        }
    }
}

int main() {
    FILE* input = fopen("data/day_04.txt", "r");
    char buffer[LINE_MAX] = { 0 };

    int cnt_valid_p1 = 0, cnt_valid_p2 = 0;
    char** words = da_create(sizeof(char*));
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        get_words(&words, buffer);
        size_t n = da_length(words);

        int(*counter)[LETTERS] = calloc(n, sizeof(*counter));
        for (size_t i = 0; i < n; ++i) {
            char* s = words[i];
            while (*s != '\0')
                counter[i][*(s++) - 'a'] += 1;
        }

        bool valid_p1, valid_p2;
        is_valid(n, words, counter, &valid_p1, &valid_p2);
        cnt_valid_p1 += valid_p1;
        cnt_valid_p2 += valid_p2;

        da_set_length(words, 0);
        free(counter);
    }
    fclose(input);
    da_free(words);

    printf("Part 1: %d\n", cnt_valid_p1);
    printf("Part 2: %d\n", cnt_valid_p2);
}