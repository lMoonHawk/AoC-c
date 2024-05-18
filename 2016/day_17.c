#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MD5_IMPLEMENTATION
#include "../common/md5.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

typedef struct {
    Point2 pos;
    int path_size;
    char* path;
} Node;

const char directions[] = "UDLR";
const Point2 moves[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

static inline bool is_door_open(int n, int room) {
    return n & (1 << room);
}

int doors_status(Node node, const char* passcode, size_t code_size) {
    char* in = calloc(code_size + node.path_size + 1, 1);
    strncat(in, passcode, code_size);
    strncat(in, node.path, node.path_size);

    uint8_t hashed[16];
    md5String(in, hashed);

    int res = 0;
    res += (hashed[0] > 0xAF) << 0;
    res += ((hashed[0] & 0xF) > 0x0A) << 1;
    res += (hashed[1] > 0xAF) << 2;
    res += ((hashed[1] & 0xF) > 0x0A) << 3;
    return res;
}

void next_node(Point2 new_pos, int door, Node current, Node* next) {
    next->pos = new_pos;
    next->path_size += current.path_size + 1;
    next->path = calloc(next->path_size + 1, 1);
    strncpy(next->path, current.path, next->path_size);
    next->path[next->path_size - 1] = directions[door];
}

int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    char passcode[LINE_MAX] = { 0 };
    if (!fread(passcode, 1, LINE_MAX, input)) exit(0);
    fclose(input);
    size_t n = strlen(passcode);

    char* shortest_path = NULL;
    int longest_path_size = 0;

    Queue nodes = q_create(sizeof(Node));
    Node init = { 0 };
    init.path = calloc(1, 1);
    q_push(nodes, init);

    while (q_length(nodes)) {
        Node current;
        q_pop(nodes, &current);

        int doors = doors_status(current, passcode, n);
        for (int i = 0; i < 4; ++i) {
            Point2 new_pos = p2add(current.pos, moves[i]);

            if (new_pos.x < 0 || new_pos.x >= 4 || new_pos.y < 0 || new_pos.y >= 4)
                continue;
            if (is_door_open(doors, i)) {
                Node next = { 0 };
                next_node(new_pos, i, current, &next);
                // Goal
                if (new_pos.x == 3 && new_pos.y == 3) {
                    if (shortest_path == NULL) {
                        shortest_path = strdup(next.path);
                    }
                    if (longest_path_size < next.path_size) {
                        longest_path_size = next.path_size;
                    }
                    continue;
                }
                q_push(nodes, next);
            }
        }
        free(current.path);
    }

    printf("Part 1: %s\n", shortest_path);
    printf("Part 2: %d\n", longest_path_size);

    free(shortest_path);
}