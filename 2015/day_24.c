#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef unsigned long long ull;
typedef long long ll;

void parse_input(FILE* input, int** packages) {
    char buffer[LINE_MAX];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int package = atoi(buffer);
        da_append(*packages, package);
    }
}

void qe_min_packages(int* packages, ll sum, ull size, ull qe, size_t index, ull* min_size, ull* min_size_qe) {
    if (index == da_length(packages)) {
        if (sum == 0) {
            if (size < *min_size) {
                *min_size = size;
                *min_size_qe = qe;
            }
            else if (size == *min_size && qe < *min_size_qe) {
                *min_size_qe = qe;
            }
        }
        return;
    }
    // Exclude
    qe_min_packages(packages, sum, size, qe, index + 1, min_size, min_size_qe);
    if (sum - packages[index] >= 0) {
        // Include
        qe_min_packages(packages, sum - packages[index], size + 1, qe * packages[index], index + 1, min_size, min_size_qe);
    }
    return;
}

int main() {
    FILE* input = fopen("data/day_24.txt", "r");
    int* packages = da_create(sizeof(int));
    parse_input(input, &packages);

    ull sum = 0;
    for (size_t i = 0; i < da_length(packages); ++i)
        sum += packages[i];

    ull min_size1 = ULLONG_MAX;
    ull min_size_qe1 = ULLONG_MAX;
    ull min_size2 = ULLONG_MAX;
    ull min_size_qe2 = ULLONG_MAX;
    qe_min_packages(packages, sum / 3, 0, 1, 0, &min_size1, &min_size_qe1);
    qe_min_packages(packages, sum / 4, 0, 1, 0, &min_size2, &min_size_qe2);

    printf("Part 1: %llu\n", min_size_qe1);
    printf("Part 2: %llu\n", min_size_qe2);
    
    da_free(packages);
    fclose(input);
}