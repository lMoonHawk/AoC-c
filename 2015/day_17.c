#include <stdio.h>
#include <stdlib.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int nb_combin(int total, size_t n, int cont[n], size_t i, int cont_cnt, int* min, int* min_cnt) {
    if (i == n) {
        if (total == 0) {
            if (cont_cnt < *min) {
                *min_cnt = 1;
                *min = cont_cnt;
            }
            else if (cont_cnt == *min) {
                *min_cnt += 1;
            }
            return 1;
        }
        return 0;
    }

    int current = nb_combin(total, n, cont, i + 1, cont_cnt, min, min_cnt);
    current += nb_combin(total - cont[i], n, cont, i + 1, cont_cnt + 1, min, min_cnt);
    return current;
}

int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    int* containers = da_create(sizeof(int));

    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int container = atoi(buffer);
        da_append(containers, container);
    }

    int min = INT_MAX;
    int min_cnt = 0;
    int total = nb_combin(150, da_length(containers), containers, 0, 0, &min, &min_cnt);

    printf("Part 1: %d\n", total);
    printf("Part 2: %d", min_cnt);
    fclose(input);
}