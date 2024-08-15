#include <stdio.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef enum {
    START,
    END,
    SMALL_CAVE,
    BIG_CAVE,
} Type;

typedef struct {
    Type type;
    int* connections;
} Node;

Type get_type(char* cave) {
    if (strcmp(cave, "start") == 0)
        return START;
    else if (strcmp(cave, "end") == 0)
        return END;
    else if (cave[0] >= 'a' && cave[0] <= 'z')
        return SMALL_CAVE;
    return BIG_CAVE;
}

int get_indexof(char(**caves)[16], char cave[16], Node** map) {
    for (size_t i = 0; i < da_length(*caves); i++)
        if (strcmp((*caves)[i], cave) == 0)
            return i;
    int idx = da_length(*caves);
    _da_append((void**)caves, cave);
    Node node = { .type = get_type(cave), .connections = da_create(sizeof(int)) };
    da_append(*map, node);
    return idx;
}

Node* parse_input(FILE* input, int* start_idx) {
    Node* map = da_create(sizeof(*map));
    char(*caves)[16] = da_create(16);

    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char cave1[16] = { 0 };
        char cave2[16] = { 0 };
        strcpy(cave1, strtok(buffer, "-"));
        strcpy(cave2, strtok(NULL, "\r\n"));

        int cave1_idx = get_indexof(&caves, cave1, &map);
        int cave2_idx = get_indexof(&caves, cave2, &map);
        da_append(map[cave1_idx].connections, cave2_idx);
        da_append(map[cave2_idx].connections, cave1_idx);
        if (map[cave1_idx].type == START)
            *start_idx = cave1_idx;
    }
    da_free(caves);
    return map;
}

int traverse_rec(Node* map, int idx, int quota, int* visited) {
    if (map[idx].type == END) {
        free(visited);
        return 1;
    }
    if (map[idx].type == SMALL_CAVE)
        ++visited[idx];
    int paths = 0;
    for (size_t i = 0; i < da_length(map[idx].connections); ++i) {
        int new_idx = map[idx].connections[i];
        if (map[new_idx].type == START)
            continue;
        int new_quota = quota;
        if (visited[new_idx] > 0) {
            if (new_quota == 0)
                continue;
            --new_quota;
        }
        int* new_visited = malloc(sizeof(int) * da_length(map));
        memcpy(new_visited, visited, sizeof(int) * da_length(map));
        paths += traverse_rec(map, new_idx, new_quota, new_visited);
    }
    free(visited);
    return paths;
}

int traverse(Node* map, int idx, int quota) {
    int* visited = calloc(da_length(map), sizeof(int));
    return traverse_rec(map, idx, quota, visited);
}

int main() {
    FILE* input = fopen("data/day_12.txt", "r");
    int start_idx;
    Node* map = parse_input(input, &start_idx);
    fclose(input);

    int paths_p1 = traverse(map, start_idx, 0);
    int paths_p2 = traverse(map, start_idx, 1);

    for (size_t i = 0; i < da_length(map); ++i)
        da_free(map[i].connections);
    da_free(map);

    printf("Part 1: %d\n", paths_p1);
    printf("Part 2: %d\n", paths_p2);
}