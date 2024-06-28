#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

typedef struct {
    Point3 position;
    int radius;
} Bot;

typedef struct {
    Point3 position;
    int in_range, edge, distance;
} Node;

static inline int max(int a, int b) {
    return a > b ? a : b;
}

static inline int min(int a, int b) {
    return a < b ? a : b;
}

int nodecmp(const void* a, const void* b) {
    Node* na = (Node*)a;
    Node* nb = (Node*)b;
    if (na->in_range == nb->in_range) {
        if (na->edge == nb->edge) {
            return nb->distance - na->distance;
        }
        return na->edge - nb->edge;
    }
    return na->in_range - nb->in_range;
}

// Counts the number of bots that could reach the cube (point, point + edge)
int estimate_in_range(Bot* bots, Point3 point, int edge) {
    int in_range = 0;
    Point3 closest = point;
    for (size_t i = 0; i < da_length(bots); ++i) {
        Bot b = bots[i];
        Point3 bp = b.position;
        // Calulate the point on the cube closest to the bot
        closest.x = (bp.x < (point.x + edge / 2) ? max(bp.x, point.x) : min(bp.x, point.x + edge));
        closest.y = (bp.y < (point.y + edge / 2) ? max(bp.y, point.y) : min(bp.y, point.y + edge));
        closest.z = (bp.z < (point.z + edge / 2) ? max(bp.z, point.z) : min(bp.z, point.z + edge));
        // The bot could reach the cube if its distance to the closest point on the cube is less than its range
        if (p3dist(bp, closest) <= b.radius)
            ++in_range;
    }
    return in_range;
}

Bot* parse_input(FILE* input) {
    char* line_fmt = "pos=<%d,%d,%d>, r=%d";
    Bot* bots = da_create(sizeof(*bots));
    Bot bot;
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        sscanf(buffer, line_fmt, &bot.position.x, &bot.position.y, &bot.position.z, &bot.radius);
        da_append(bots, bot);
    }
    return bots;
}

int main() {
    FILE* input = fopen("data/day_23.txt", "r");
    Bot* bots = parse_input(input);
    fclose(input);

    Bot* strongest = &bots[0];
    for (size_t i = 1; i < da_length(bots); ++i)
        if (bots[i].radius > strongest->radius)
            strongest = &bots[i];

    int strongest_in_range = 0;
    for (size_t i = 0; i < da_length(bots); ++i)
        strongest_in_range += p3dist(strongest->position, bots[i].position) <= strongest->radius;

    int bounds = 0;
    for (size_t i = 0; i < da_length(bots); ++i) {
        if (abs(bots[i].position.x) > bounds) bounds = abs(bots[i].position.x);
        if (abs(bots[i].position.y) > bounds) bounds = abs(bots[i].position.y);
        if (abs(bots[i].position.z) > bounds) bounds = abs(bots[i].position.z);
    }
    // Cubes are described by their min coordinates (left-bottom-back point).
    // Make a cube side length the first power of 2 that contains all bots.
    int edge = 1;
    while (edge <= bounds)
        edge *= 2;
    Point3 point = { .x = -edge, .y = -edge, .z = -edge };
    edge *= 2;

    // The priority queue maximises the amount of bots in the cube then the size of the cube and finally minimises the distance to origin.
    Pqueue pq = pq_create(sizeof(Node), nodecmp);
    Node current = { point, da_length(bots), edge, p3dist(point, (Point3) { 0 }) };
    pq_push(pq, current);
    while (pq_length(pq)) {
        pq_pop(pq, &current);
        if (current.edge == 0)
            break;
        current.edge /= 2;
        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                for (int z = 0; z < 2; ++z) {
                    Point3 offset = { x, y, z };
                    Point3 subpoint = p3add(current.position, p3scale(offset, current.edge));
                    int in_range = estimate_in_range(bots, subpoint, current.edge);
                    Node next = { subpoint, in_range, current.edge, p3dist(subpoint, (Point3) { 0 }) };
                    pq_push(pq, next);
                }
            }
        }
    }
    da_free(bots);
    pq_free(pq);
    printf("Part 1: %d\n", strongest_in_range);
    printf("Part 2: %d\n", current.distance);
}