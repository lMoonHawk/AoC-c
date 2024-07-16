#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

Point2 p2adjacents[4] = {
    {.x = 0, .y = -1},
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0}
};

typedef struct {
    Point2 position;
    uint32_t doors;
} Path;

typedef struct {
    char cell;
    Point2 position;
} Node;

typedef struct {
    int steps;
    uint32_t doors;
} Edge;

typedef struct {
    char position[4];
    uint32_t keys;
} Robot;

typedef struct {
    Robot robot;
    int steps;
} State;

int state_cmp(const void* a, const void* b) {
    return ((State*)b)->steps - ((State*)a)->steps;
}

void build_graph_from_node(Point2 position, char* vault, int stride, Edge* edges) {
    Queue q = q_create(sizeof(Path));
    Path init = {.position = position, .doors = 0 };
    q_push(q, init);
    Hset* visited = hs_create(sizeof(Point2));
    int depth_nodes = 1;
    int steps = 0;
    while (q_length(q)) {
        if (depth_nodes == 0) {
            depth_nodes = q_length(q);
            ++steps;
        }
        Path current;
        q_pop(q, &current);
        if (hs_contains(*visited, &current.position)) {
            --depth_nodes;
            continue;
        }
        hs_insert(visited, &current.position);
        char current_cell = vault[current.position.y * stride + current.position.x];
        if ('a' <= current_cell && current_cell <= 'z') {
            edges[(int)current_cell].steps = steps;
            edges[(int)current_cell].doors = current.doors;
        }
        for (int i = 0; i < 4; ++i) {
            Path next = current;
            next.position = p2add(current.position, p2adjacents[i]);
            char cell = vault[next.position.y * stride + next.position.x];
            if (cell == '#')
                continue;
            else if ('A' <= cell && cell <= 'Z')
                next.doors |= 1 << (cell - 'A');
            q_push(q, next);
        }
        --depth_nodes;
    }
    q_free(q);
    hs_free(visited);
}

void* build_graph(Node* nodes, char* vault, int stride) {
    Edge* edges = calloc(128 * 128, sizeof(Edge));
    for (size_t i = 0; i < da_length(nodes); ++i)
        build_graph_from_node(nodes[i].position, vault, stride, &edges[nodes[i].cell * 128]);
    return edges;
}

int get_min_steps(Edge(*edges)[128], int robot_cnt, char start_cell[robot_cnt], uint32_t all_keys) {
    Pqueue pq = pq_create(sizeof(State), state_cmp);
    State init = { 0 };
    for (int r = 0; r < robot_cnt; ++r)
        init.robot.position[r] = start_cell[r];
    pq_push(pq, init);
    Hset* visited = hs_create(sizeof(Robot));
    int result = -1;
    while (pq_length(pq)) {
        State current;
        pq_pop(pq, &current);
        if (hs_contains(*visited, &current.robot))
            continue;
        hs_insert(visited, &current.robot);
        if (current.robot.keys == all_keys) {
            result = current.steps;
            break;
        }
        for (int r = 0; r < robot_cnt; ++r) {
            for (int i = 0; i < 128; ++i) {
                State next = current;
                next.robot.position[r] = i;
                Edge path = edges[(int)current.robot.position[r]][(int)next.robot.position[r]];
                if (!path.steps)
                    continue;
                next.steps += path.steps;
                if (~next.robot.keys & path.doors)
                    continue;
                if (next.robot.keys & (1 << (i - 'a')))
                    continue;
                next.robot.keys |= 1 << (i - 'a');
                pq_push(pq, next);
            }
        }
    }
    hs_free(visited);
    pq_free(pq);
    return result;
}

char* parse_input(FILE* input, int* stride, Point2* position, Node** nodes) {
    *nodes = da_create(sizeof(Node));
    char* vault = da_create(1);
    char c = fgetc(input);
    int x = 0, y = 0;
    while (c != EOF) {
        if (c == '\n') {
            *stride = x;
            x = 0;
            ++y;
        } else if (c == '\r') {
        } else {
            da_append(vault, c);
            if ('a' <= c && c <= 'z') {
                da_append(*nodes, ((Node){.cell = c, .position = (Point2){ .x = x, .y = y } }));
            } else if (c == '@') {
                *position = (Point2){ .x = x, .y = y };
                da_append(*nodes, ((Node){.cell = c, .position = (Point2){ .x = x, .y = y } }));
            }
            ++x;
        }
        c = fgetc(input);
    }
    return vault;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");
    int stride;
    Point2 position;
    Node* nodes;
    char* vault = parse_input(input, &stride, &position, &nodes);
    fclose(input);

    // Keys are stored in a bitset
    uint32_t all_keys = 1;
    for (size_t i = 0; i < da_length(nodes) - 1; ++i)
        all_keys *= 2;
    --all_keys;


    Edge(*edges)[128] = build_graph(nodes, vault, stride);
    char start_cell_p1[4] = { '@', };
    int min_steps_p1 = get_min_steps(edges, 1, start_cell_p1, all_keys);
    free(edges);

    char robot_index = '0';
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int index = (position.y + dy) * stride + (position.x + dx);
            if (dx == 0 || dy == 0) {
                vault[index] = '#';
            } else {
                Node robot = { .cell = robot_index++, .position = (Point2){.x = position.x + dx, .y = position.y + dy } };
                da_append(nodes, robot);
            }
        }
    }
    Edge(*edges_p2)[128] = build_graph(nodes, vault, stride);
    char start_cell_p2[4] = { '0', '1', '2', '3' };
    int min_steps_p2 = get_min_steps(edges_p2, 4, start_cell_p2, all_keys);
    free(edges_p2);
    da_free(nodes);
    da_free(vault);

    printf("Part 1: %d\n", min_steps_p1);
    printf("Part 2: %d\n", min_steps_p2);
}