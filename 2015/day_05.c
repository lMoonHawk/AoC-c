#include <stdio.h>
#include <string.h>

int is_nice_part1(const char* str) {
    char banned[4][3] = { "ab", "cd", "pq", "xy" };

    for (int i = 0; i < 4; i++) {
        if (strstr(str, banned[i]) != NULL) {
            return 0;
        }
    }
    char last_char;
    int cons_char = 0;
    int vowels = 0;
    for (int i = 0; str[i] != 0; i++) {
        vowels += (str[i] == 'a' || str[i] == 'e' || str[i] == 'i' || str[i] == 'o' || str[i] == 'u');
        if (i > 0) cons_char += (last_char == str[i]);
        last_char = str[i];
    }
    if (vowels >= 3 && cons_char > 0) {
        return 1;
    }
    return 0;
}

int is_nice_part2(const char* str) {
    size_t n = strlen(str);

    int cond1 = 0;
    int cond2 = 0;
    for (int i = 0; i < n - 2; i++) {
        cond2 += (str[i] == str[i+2]);
        for (int j = i + 2; j < n - 1; j++) {
            cond1 += (str[i] == str[j] && str[i + 1] == str[j + 1]);
        }
    }
    return cond1 && cond2;
}


int main() {
    char buffer[20];
    int nice_cnt_1 = 0;
    int nice_cnt_2 = 0;
    FILE* input = fopen("data/day_05.txt", "r");

    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        nice_cnt_1 += is_nice_part1(buffer);
        nice_cnt_2 += is_nice_part2(buffer);
    }
    fclose(input);
    printf("Part 1: %d\n", nice_cnt_1);
    printf("Part 2: %d", nice_cnt_2);

    return 0;
}

