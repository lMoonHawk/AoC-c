#include <stdio.h>
#include <string.h>

int main() {
    FILE* input = fopen("data/day_08.txt", "r");
    char buffer[100];
    size_t part1 = 0, part2 = 0;

    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        size_t evaled = 0;
        size_t n = strlen(buffer);
        size_t k;
        for (k = 1; k < n - 1; k++) {
            if (buffer[k] == '\\') {
                k += (buffer[k + 1] == 'x') ? 3 : 1;
            }
            evaled += 1;
        }
        size_t spe_cnt = 0;
        for (size_t i = 0; i < n; i++) {
            if (buffer[i] == '\\' || buffer[i] == '"') spe_cnt++;
        }
        part1 += n - evaled;
        part2 += 2 + spe_cnt;
    }
    printf("Part 1: %zu\n", part1);
    printf("Part 2: %zu", part2);
    fclose(input);
    return 0;
}

