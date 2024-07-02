#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define LINE_MAX 256

static inline int arr4_manhattan(int a[4], int b[4]) {
    return abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2]) + abs(a[3] - b[3]);
}

void collect_constellation(Hmap* graph, int(*points)[4], Hset* visited, int i) {
    hs_insert(visited, &i);
    int** values = hm_search(*graph, &i);
    if (values != NULL) {
        for (size_t j = 0; j < da_length(*values); ++j) {
            if (!hs_contains(*visited, &(*values)[j])) {
                collect_constellation(graph, points, visited, (*values)[j]);
            }
        }
    }
}

void insert_node(Hmap** graph, int i, int j) {
    int** values = hm_search(**graph, &i);
    if (values == NULL) {
        hm_insert(*graph, &i, &(int*){da_create(sizeof(int))});
        values = hm_search(**graph, &i);
    }
    da_append(*values, j);
}

void* parse_input(FILE* input, Hmap** graph) {
    int(*points)[4] = da_create(sizeof(*points));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int point[4];
        sscanf(buffer, "%d,%d,%d,%d", &point[0], &point[1], &point[2], &point[3]);
        da_append(points, *point);
    }
    *graph = hm_create(sizeof(int), sizeof(int*));
    for (size_t i = 0; i < da_length(points) - 1; ++i) {
        for (size_t j = i + 1; j < da_length(points); ++j) {
            if (arr4_manhattan(points[i], points[j]) <= 3) {
                insert_node(graph, i, j);
                insert_node(graph, j, i);
            }
        }
    }
    return points;
}

int main() {
    FILE* input = fopen("data/day_25.txt", "r");

    Hmap* graph = NULL;
    int(*points)[4] = parse_input(input, &graph);
    fclose(input);

    Hset* visited = hs_create(sizeof(int));
    int constellations_count = 0;
    for (size_t i = 0; i < da_length(points); ++i) {
        if (!hs_contains(*visited, &i)) {
            collect_constellation(graph, points, visited, i);
            ++constellations_count;
        }
    }

    da_free(points);
    hs_free(visited);
    for (size_t i = 0; i < graph->capacity; ++i)
        if (graph->htable[i].value != NULL)
            da_free(*(int**)graph->htable[i].value);
    hm_free(graph);

    printf("Part 1: %d\n", constellations_count);
    printf("Part 2: \n");
}