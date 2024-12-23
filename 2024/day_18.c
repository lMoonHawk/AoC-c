#include <stdio.h>
#include <stdbool.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"

#define _LINE_MAX 256

#define SIZE 71
#define KB 1024

int get_min_steps(bool grid[SIZE][SIZE]) {
    bool visited[SIZE][SIZE] = { 0 };
    Point2 start = { .x = 0, .y = 0 };
    Queue pq = q_create(sizeof(Point2));
    q_push(pq, start);

    Point2 current;
    bool exited = false;
    size_t depth = 1;
    int steps = 0;
    while (q_length(pq)) {
        if (depth == 0) {
            ++steps;
            depth = q_length(pq);
        }
        q_pop(pq, &current);
        if (current.x == SIZE - 1 && current.y == SIZE - 1) {
            exited = true;
            break;
        }

        for (P2Direction i = 0; i < P2_ADJCOUNT; i++) {
            Point2 next = p2add(current, p2adjacents[i]);
            if (next.x >= 0 && next.x < SIZE && next.y >= 0 && next.y < SIZE && !grid[next.y][next.x]) {
                if (!visited[next.y][next.x]) {
                    visited[next.y][next.x] = true;
                    q_push(pq, next);
                }
            }
        }
        --depth;
    }
    q_free(pq);
    return exited ? steps : 0;
}

int main() {
    FILE* input = fopen("data/day_18.txt", "r");
    bool grid[SIZE][SIZE] = { 0 };

    char buffer[_LINE_MAX] = { 0 };
    int x, y;
    int bytes = 0;
    int min_steps_kb = 0;
    Point2 unreachable_point = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, "%d,%d", &x, &y);
        grid[y][x] = true;
        if (++bytes >= KB) {
            int a = get_min_steps(grid);
            if (bytes == KB) {
                min_steps_kb = a;
            } else if (!a) {
                unreachable_point = (Point2){ .x = x, .y = y };
                break;
            }
        }
    }
    fclose(input);

    printf("Part 1: %d\n", min_steps_kb);
    printf("Part 2: %d,%d\n", unreachable_point.x, unreachable_point.y);
}