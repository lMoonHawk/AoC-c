#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINE_MAX 256
#define LETTERS 26

typedef struct {
    char c;
    int cnt; 
} Counter;

int cmp_counter(const void* _c1, const void* _c2) {
    Counter c1 = *(Counter*) _c1;
    Counter c2 = *(Counter*) _c2;
    if (c1.cnt == c2.cnt)
        return c1.c - c2.c;
    return c2.cnt - c1.cnt;
}

bool is_valid(const char *checksum, Counter *c) {
    int ok = 0;
    for (int i = 0; i < 5; ++i) if (c[i].c == checksum[i]) ++ok;
    return (ok == 5);
}

void decrypt(char *msg, int shift) {
    size_t n = strlen(msg);
    for (size_t i = 0; i < n; ++i) {
        if (msg[i] != ' ') msg[i] = (msg[i] - 'a' + shift) % 26 + 'a';
    }
}

int main() {
    FILE* input = fopen("data/day_04.txt", "r");

    int valid_cnt = 0;
    int id_np = -1;

    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        char *name = calloc(strlen(buffer), 1);
        int hm_letters[LETTERS] = {0};

        int id = 0;
        bool checksum = false;

        char *split = strtok(buffer, "-[]\n");        
        while (split != NULL) {
            if (checksum) {
                Counter ordered[LETTERS] = {0};
                size_t n = 0;
                for (int i = 0; i < LETTERS; ++i) {
                    if(hm_letters[i] > 0) {
                        ordered[n++] = (Counter) {.c = i + 'a', .cnt = hm_letters[i]};
                    }
                }
                qsort(ordered, n, sizeof(Counter), cmp_counter);
                if (is_valid(split, ordered)) {
                    valid_cnt += id;
                    decrypt(name, id);
                    if (strstr(name, "north") != NULL) {
                        id_np = id;
                    }
                }
                break;
            }
            else if ((id = atoi(split))) {
                checksum = true;
            }
            else {
                strcat(name, split);
                strcat(name, " ");
                while(*split != '\0') 
                    ++hm_letters[*(split++) - 'a'];              
            }
            split = strtok(NULL, "-[]\n");
        }
        free(name);
    }
    
    printf("Part 1: %d\n", valid_cnt);
    printf("Part 2: %d\n", id_np);
    
    fclose(input);
}