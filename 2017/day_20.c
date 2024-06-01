#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int x, y, z;
} Point3;

int abs(int x) {
    return x >= 0 ? x : -x;
}

Point3 p3add(Point3 p1, Point3 p2) {
    return (Point3) { p1.x + p2.x, p1.y + p2.y, p1.z + p2.z };
}

int p3cmp(Point3 p1, Point3 p2) {
    if (p1.x != p2.x) return p1.x - p2.x;
    if (p1.y != p2.y) return p1.y - p2.y;
    return p1.z - p2.z;
}

int p3dist(Point3 p1, Point3 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y) + abs(p1.z - p2.z);
}


typedef struct {
    Point3 p, v, a;
} Particule;

Point3 particule_update(Particule* particule) {
    particule->v = p3add(particule->v, particule->a);
    particule->p = p3add(particule->p, particule->v);
    return particule->p;
}

bool sign(int p, int v, int a) {
    bool same_sign = (p > 0) == (v > 0) && (p > 0) == (a > 0);
    same_sign |= (p > 0) == (v > 0) && (p > 0) == (a == 0);
    same_sign |= (p > 0) == (v == 0) && (p > 0) == (a == 0);
    return same_sign;
}

bool particule_escaping(Particule part) {
    return sign(part.p.x, part.v.x, part.a.x)
        && sign(part.p.y, part.v.y, part.a.y)
        && sign(part.p.z, part.v.z, part.a.z);
}


int revintcmp(const void* i1, const void* i2) {
    return *(int*)i2 - *(int*)i1;
}

Point3 parse_p3(void) {
    Point3 p3 = { 0 };
    p3.x = atoi(strtok(NULL, ","));
    p3.y = atoi(strtok(NULL, ","));
    p3.z = atoi(strtok(NULL, "<"));
    return p3;
}

Particule* parse_input(FILE* input) {
    Particule* particules = da_create(sizeof(*particules));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        strtok(buffer, "<");
        da_append(particules, ((Particule) { parse_p3(), parse_p3(), parse_p3() }));
    }
    return particules;
}

int main() {
    FILE* input = fopen("data/day_20.txt", "r");
    Particule* particules = parse_input(input);
    fclose(input);

    size_t closest_i = 0;
    int min_dist = p3dist(particules[0].a, (Point3) { 0 });
    for (size_t i = 1; i < da_length(particules); ++i) {
        int dist = p3dist(particules[i].a, (Point3) { 0 });
        if (dist < min_dist) {
            min_dist = dist;
            closest_i = i;
        }
    }

    bool* collided = calloc(da_length(particules), sizeof(*collided));
    size_t* collided_this_step = da_create(sizeof(*collided_this_step));

    bool stop = false;
    int alive_cnt = da_length(particules);
    while (!stop) {
        stop = true;
        for (size_t i = 0; i < da_length(particules); ++i) {
            if (collided[i])
                continue;
            particule_update(&particules[i]);
            if (!particule_escaping(particules[i]))
                stop = false;
            for (size_t j = 0; j < i; ++j) {
                if (!collided[j] && p3cmp(particules[i].p, particules[j].p) == 0) {
                    da_append(collided_this_step, i);
                    da_append(collided_this_step, j);
                }
            }
        }
        /* Recording indexes of collided particules is needed to ensure we are both:
            - Not colliding with zombie particules
            - Allowing colliding with particules that have already collided this same turn*/
        for (size_t i = 0; i < da_length(collided_this_step); ++i) {
            if (!collided[collided_this_step[i]])
                --alive_cnt;
            collided[collided_this_step[i]] = true;
        }
        da_set_length(collided_this_step, 0);
    }
    da_free(collided_this_step);
    da_free(particules);
    free(collided);

    printf("Part 1: %zu\n", closest_i);
    printf("Part 2: %d\n", alive_cnt);
}