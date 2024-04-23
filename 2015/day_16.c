#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 256


const char* const compounds[10] = {
    "children",
    "cats",
    "samoyeds",
    "pomeranians",
    "akitas",
    "vizslas",
    "goldfish",
    "trees",
    "cars",
    "perfumes",
};

int comptokey(const char* comp) {
    for (int i = 0; i < 10; ++i) if (strcmp(comp, compounds[i]) == 0) return i;
    return -1;
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    short target[] = { 3, 7, 2, 3, 0, 0, 5, 3, 2, 1 };

    int sue_nb = 1;
    char buffer[LINE_MAX];

    int sue_id_p1 = -1;
    int sue_id_p2 = -1;
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        short sue[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

        strtok(buffer, " ");
        strtok(NULL, " ");
        char* comp = strtok(NULL, ": ");
        while (comp != NULL) {
            short value = atoi(strtok(NULL, ": "));
            sue[comptokey(comp)] = value;
            comp = strtok(NULL, ": ");
        }

        int correct_p1 = 1;
        for (int i = 0; i < 10; ++i) {
            if (sue[i] != -1 && sue[i] != target[i]) {
                correct_p1 = 0;
                break;
            }
        }
        int correct_p2 = 1;
        for (int i = 0; i < 10; ++i) {
            if (sue[i] == -1) continue;
            switch (i) {
                case 1: case 7:
                    if (sue[i] <= target[i]) correct_p2 = 0;
                    break;
                case 3: case 6:
                    if (sue[i] >= target[i]) correct_p2 = 0;
                    break;
                default:
                    if (sue[i] != target[i]) correct_p2 = 0;
            }
            if (correct_p2 == 0) break;
        }
        if (correct_p1) sue_id_p1 = sue_nb;
        if (correct_p2) sue_id_p2 = sue_nb;
        if (sue_id_p1 != -1 && sue_id_p2 != -1) break;
        ++sue_nb;
    }
    printf("Part 1: %d\n", sue_id_p1);
    printf("Part 2: %d", sue_id_p2);
    fclose(input);
}