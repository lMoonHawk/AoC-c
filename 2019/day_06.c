#include <stdio.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"

#define LINE_MAX 256

int get_steps(Hmap* graph, char* i, char* j) {
    Hset* visited = hs_create(HM_STRING);
    Queue queue = q_create(sizeof(char*));
    q_push(queue, i);
    int depth_nodes = 1;
    int steps = 0;
    while (q_length(queue)) {
        if (depth_nodes == 0) {
            depth_nodes = q_length(queue);
            ++steps;
        }
        char* obj;
        q_pop(queue, &obj);
        --depth_nodes;
        if (strcmp(obj, j) == 0)
            break;
        char*** transfers = hm_search(*graph, obj);
        for (size_t i = 0; i < da_length(*transfers); ++i) {
            char* transfer = (*transfers)[i];
            if (!hs_contains(*visited, transfer)) {
                hs_insert(visited, transfer);
                q_push(queue, transfer);
            }
        }
    }
    hs_free(visited);
    q_free(queue);
    return steps;
}

void free_graph(Hmap* graph) {
    for (size_t i = 0; i < graph->capacity; ++i) {
        if (graph->htable[i].key != NULL) {
            char*** transfers = graph->htable[i].value;
            for (size_t j = 0; j < da_length(*transfers); ++j)
                free((*transfers)[j]);
            da_free(*transfers);
        }
    }
    hm_free(graph);
}

void insert_node(Hmap* graph, char* i, char* j) {
    char*** transfers = hm_search(*graph, i);
    if (transfers == NULL)
        transfers = hm_insert(graph, i, &(char**){da_create(sizeof(char*))});
    da_append(*transfers, (char*) { strdup(j) });
}

Hmap* parse_input(FILE* input) {
    Hmap* graph = hm_create(HM_STRING, sizeof(char**));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        char* orbited = strtok(buffer, ")");
        char* orbiter = strtok(NULL, "\n\r");
        insert_node(graph, orbiter, orbited);
        insert_node(graph, orbited, orbiter);
    }
    return graph;
}

int main() {
    FILE* input = fopen("data/day_06.txt", "r");
    Hmap* graph = parse_input(input);
    fclose(input);

    int checksum = 0;
    for (size_t i = 0; i < graph->capacity; ++i)
        if (graph->htable[i].key != NULL)
            checksum += get_steps(graph, (char*)graph->htable[i].key, "COM");

    int you_to_santa = get_steps(graph, "YOU", "SAN") - 2;

    free_graph(graph);

    printf("Part 1: %d\n", checksum);
    printf("Part 2: %d\n", you_to_santa);
}