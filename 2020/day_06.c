#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define LINE_MAX 256
#define CHARS 128

int main() {
    FILE* input = fopen("data/day_06.txt", "r");

    int anyone = 0;
    int everyone = 0;
    int answers[CHARS] = { 0 };
    int in_grp_cnt = 0;

    char buffer[LINE_MAX] = { 0 };
    while (true) {
        if (fgets(buffer, sizeof(buffer), input) == NULL || buffer[0] == '\n' || buffer[0] == '\r') {
            for (int i = 0; i < CHARS; ++i) {
                everyone += answers[i] == in_grp_cnt;
                anyone += answers[i] > 0;
            }
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                memset(answers, 0, CHARS * sizeof(int));
                in_grp_cnt = 0;
            } else {
                break;
            }
        } else {
            ++in_grp_cnt;
            for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != '\r'; ++i)
                answers[(int)buffer[i]] += 1;
        }
    }
    fclose(input);

    printf("Part 1: %d\n", anyone);
    printf("Part 2: %d\n", everyone);
}