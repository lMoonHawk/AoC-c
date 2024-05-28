#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

int count_nodes(int** nodes, int index, bool* visited) {
    visited[index] = true;
    int cnt = 1;
    for (size_t i = 0; i < da_length(nodes[index]); ++i) {
        int next = nodes[index][i];
        if (!visited[next]) {
            cnt += count_nodes(nodes, next, visited);
        }
    }
    return cnt;
}


int** parse_input(FILE* input) {
    int** pgs = da_create(sizeof(*pgs));

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int* piped = da_create(sizeof(*piped));
        strtok(buffer, " ");
        strtok(NULL, " ");
        char* tok = strtok(NULL, ", ");
        while (tok != NULL) {
            da_append(piped, (int) { atoi(tok) });
            tok = strtok(NULL, ", ");
        }
        da_append(pgs, piped);
    }
    return pgs;
}

int main() {
    FILE* input = fopen("data/day_12.txt", "r");
    int** pgs = parse_input(input);
    fclose(input);

    size_t n = da_length(pgs);
    bool* visited = calloc(n, sizeof(*visited));
    
    int grp_zero_cnt = count_nodes(pgs, 0, visited);
    int grp_cnt = 1;
    for (size_t i = 1; i < n; ++i) {
        if (!visited[i]) {
            count_nodes(pgs, i, visited);
            grp_cnt += 1;
        }
    }
    for (size_t i = 0; i < da_length(pgs); ++i)
        da_free(pgs[i]);
    da_free(pgs);
    free(visited);

    printf("Part 1: %d\n", grp_zero_cnt);
    printf("Part 2: %d\n", grp_cnt);
}