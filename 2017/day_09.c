#include <stdio.h>
#include <stdbool.h>

int main() {
    FILE* input = fopen("data/day_09.txt", "r");

    int grp_depth = 0, score = 0, garbage_chars_cnt = 0;
    bool in_garbage = false, escape = false;

    char c = fgetc(input);
    while (c != EOF && c != '\r' && c != '\n') {
        if (escape)
            escape = false;
        else if (c == '!')
            escape = true;
        else if (c == '>')
            in_garbage = false;
        else if (in_garbage)
            ++garbage_chars_cnt;
        else if (c == '<')
            in_garbage = true;
        else if (c == '{')
            ++grp_depth;
        else if (c == '}')
            score += grp_depth--;
        c = fgetc(input);
    }
    fclose(input);

    printf("Part 1: %d\n", score);
    printf("Part 1: %d\n", garbage_chars_cnt);
}