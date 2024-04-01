#include <stdio.h>
#include <string.h>

void increment(char* str) {
    for (size_t i = strlen(str) - 1; i > 0; i--) {
        str[i]++;
        if (str[i] <= 'z') break;
        else str[i] = 'a';
    }
}

int is_valid(char* str) {
    size_t pairs = 0;
    int pair_used = 0;
    size_t consecutive = 0;
    size_t n = strlen(str);

    for (size_t i = 0; i < n; i++) {
        // Passwords may not contain the letters i, o, or l
        if (str[i] == 'i' || str[i] == 'o' || str[i] == 'l') return 0;
        // Passwords must contain at least two different, non-overlapping pairs of letters
        if (i < n - 1 && pair_used == 0 && str[i] == str[i + 1]) {
            pairs++;
            pair_used = 1;
        }
        else {
            pair_used = 0;
        }
        // Passwords must include one increasing straight of at least three letters
        if (i < n - 2 && (str[i] + 1) == str[i + 1] && (str[i + 1] + 1) == str[i + 2]) consecutive++;
    }
    return (pairs >= 2) && consecutive;
}

int main() {
    FILE* input = fopen("data/day_11.txt", "r");
    char pw[9];
    fgets(pw, sizeof(pw), input);

    unsigned int cnt = 0;
    while (cnt < 2) {
        increment(pw);
        if (is_valid(pw)) {
            cnt++;
            printf("Part %d: %s\n", cnt, pw);
        }
    }

    return 0;
}
