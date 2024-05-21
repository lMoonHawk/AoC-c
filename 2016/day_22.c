#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"

#define LINE_MAX 256

typedef struct {
    int x, y, size, used;
} Node;

Node* parse_input(FILE* input) {
    Node* nodes = da_create(sizeof(Node));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n') {
        // /dev/grid/node-x0-y0     90T   69T    21T   76%
        if (buffer[0] != '/') continue;
        Node node;
        strtok(buffer, "x");
        node.x = atoi(strtok(NULL, "y"));
        node.y = atoi(strtok(NULL, " "));
        node.size = atoi(strtok(NULL, " "));
        node.used = atoi(strtok(NULL, " "));
        da_append(nodes, node);
    }
    return nodes;
}

bool in_visited(Node** visited, Node* node) {
    for (size_t i = 0; i < da_length(visited); ++i) {
        if (visited[i] == node)
            return true;
    }
    return false;
}

int get_steps(Node* nodes, Node* from, Node* to, int width, int height) {
    int dx[] = { 1, -1, 0, 0 };
    int dy[] = { 0, 0, 1, -1 };

    Queue q = q_create(sizeof(Node*));
    q_push(q, from);

    Node** visited = da_create(sizeof(Node*));

    int steps = 0;
    size_t depth_nodes = 1;
    while (q_length(q)) {
        if (depth_nodes == 0) {
            ++steps;
            depth_nodes = q_length(q);
        }
        Node* current;
        q_pop(q, &current);
        if (current == to)
            break;
        for (int i = 0; i < 4; ++i) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];
            if (nx < 0 || nx >= height || ny < 0 || ny >= width)
                continue;

            size_t index = (nx * width) + ny;
            Node* next_node = &nodes[index];

            if (in_visited(visited, next_node))
                continue;
            da_append(visited, next_node);

            if (next_node->used <= current->size)
                q_push(q, next_node);
        }
        --depth_nodes;
    }
    da_free(visited);
    q_free(q);
    return steps;
}

int main() {
    FILE* input = fopen("data/day_22.txt", "r");
    Node* nodes = parse_input(input);
    fclose(input);

    size_t n = da_length(nodes);
    int viable_cnt = 0;
    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            viable_cnt += (nodes[i].used && nodes[i].used <= (nodes[j].size - nodes[j].used));
            viable_cnt += (nodes[j].used && nodes[j].used <= (nodes[i].size - nodes[i].used));
        }
    }

    // Get minimum node.
    Node* hole = nodes;
    for (size_t i = 1; i < n; ++i) if (nodes[i].used < hole->used) hole = &nodes[i];
    // Data is listed by ys first
    size_t width = nodes[n - 1].y + 1;
    size_t height = nodes[n - 1].x + 1;
    //  Fist, move the hole from it position to being in front of the data.
    //  Then, swap position of hole and data (1 step) and get back in front (4 steps)
    Node* goal = &nodes[width * (height - 2)];
    int steps = get_steps(nodes, hole, goal, width, height) + 5 * (height - 2) + 1;
    da_free(nodes);

    printf("Part 1: %d\n", viable_cnt);
    printf("Part 2: %d\n", steps);
}