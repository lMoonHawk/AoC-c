#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define LINE_MAX 256

int apply_changes(int freq, int* changes, Hset* seen, bool *dup, int* dup_freq) {
    for (size_t i = 0; i < da_length(changes); ++i) {
            freq += changes[i];
            if (!*dup && hs_contains(*seen, &freq)) {
                *dup = true;
                *dup_freq = freq;
            }
            hs_insert(seen, &freq);
        }
    return freq;
}

int main() {
    FILE* input = fopen("data/day_01.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    int* changes = da_create(sizeof(*changes));
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r')
        da_append(changes, (int) { atoi(buffer) });
    fclose(input);

    Hset* seen = hs_create(sizeof(int));
    int freq = 0;
    int dup_freq;
    bool dup = false;
    hs_insert(seen, &freq);
    freq = apply_changes(freq, changes, seen, &dup, &dup_freq);
    int one_pass = freq;

    while(!dup)
        freq = apply_changes(freq, changes, seen, &dup, &dup_freq);
    
    hs_free(seen);
    da_free(changes);

    printf("Part 1: %d\n", one_pass);
    printf("Part 1: %d\n", dup_freq);
}