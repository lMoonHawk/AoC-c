#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
// #include <sys/types.h>

#define _LINE_MAX 256
#define LETTERS 26

// size_t whichmax(size_t n, int arr[n]) {
//     int max = INT_MIN;
//     size_t index = 0;
//     for (size_t i = 0; i < n; ++i) {
//         if (arr[i] > max) {
//             max = arr[i];
//             index = i;
//         }
//     }
//     return index;
// }

int main() {
    FILE* input = fopen("data/day_06.txt", "r");
    
    char buffer[_LINE_MAX];
    char* pbuff = fgets(buffer, sizeof(buffer), input);
    buffer[strcspn(buffer, "\n")] = '\0';

    size_t cols = strlen(buffer);

    int (*counter)[cols] = calloc(LETTERS, sizeof *counter);
    
    char *most_letters = calloc(cols + 1, sizeof(char));
    int *most_freq = calloc(cols, sizeof(int));
    while (pbuff != NULL) {
        for (size_t i = 0; i < cols; ++i){
            counter[buffer[i] - 'a'][i] += 1;
            if (counter[buffer[i] - 'a'][i] > most_freq[i]) {
                most_freq[i] = counter[buffer[i] - 'a'][i];
                most_letters[i] = buffer[i];
            }
        }
        pbuff = fgets(buffer, sizeof(buffer), input);
        buffer[strcspn(buffer, "\n")] = '\0';
    }

    char *least_letters = calloc(cols + 1, sizeof(char));
    for (size_t i = 0; i < cols; ++i) {
        int least_freq = INT_MAX;
        char least_letter = 0;
        for (size_t j = 0; j < LETTERS; ++j) {
            if (counter[j][i] < least_freq) {
                least_freq = counter[j][i];
                least_letter = j + 'a';
            }
        }
        least_letters[i] = least_letter;
    }

    printf("Part 1: %s\n", most_letters);
    printf("Part 2: %s\n", least_letters);
    
    free(most_freq);
    free(most_letters);
    free(least_letters);
    fclose(input);
}