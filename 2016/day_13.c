#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

int bit_count(int n) {
    int cnt = 0;
    while (n > 0) {
        cnt += n & 1;
        n >>= 1;
    }
    return cnt;
}

bool is_open(Point2 pos, int num) {
    int x = pos.x;
    int y = pos.y;
    return !(bit_count(x * x + 3 * x + 2 * x * y + y + y * y + num) % 2);
}

bool in_visited(Point2* visited, Point2 pos) {
    for (size_t i = 0; i < da_length(visited); ++i) {
        if (visited[i].x == pos.x && visited[i].y==pos.y)
            return true;
    }
    return false;    
}

void traverse(int num, int* min_steps, int* max_reached) {
    *min_steps = 0;
    *max_reached = 0;

    Point2 pos = { 1, 1 };
    Point2* visited = da_create(sizeof(Point2));

    Point2 moves[] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

    Queue q = q_create(sizeof(pos));
    q_push(q, pos);

    int steps = 0;
    int reached = 0;

    size_t depth_nodes = 1;
    while (q_length(q)) {
        if (depth_nodes == 0) {
            ++steps;
            depth_nodes = q_length(q);
        }
        Point2 current;
        q_pop(q, &current);

        if (*min_steps == 0 && current.x == 31 && current.y == 39)
            *min_steps = steps;
        if (*max_reached == 0 && steps == 50)
            *max_reached = reached;
        if (*min_steps > 0 && *max_reached > 0)
            break;

        for (int i = 0; i < 4; ++i) {
            Point2 next = p2add(current, moves[i]);
            if (next.x >= 0 && next.y >= 0 && is_open(next, num) && !in_visited(visited, next)) {
                da_append(visited, next);
                q_push(q, next);
                ++reached;
            }
        }
        --depth_nodes;
    }
    da_free(visited);
    q_free(q);
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);
    
    int num = atoi(buffer);
    int min_steps, max_reached;
    traverse(num, &min_steps, &max_reached);
    
    printf("Part 1: %d\n", min_steps);
    printf("Part 2: %d\n", max_reached);

}