#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// https://topaz.github.io/paste/#XQAAAQCyBQAAAAAAAAA0m0pnuFI8c+fPp4GmVpXhPW2mfPreqyo8qDEZ4KCukrE8oI/ZNr2dI87tu5qiie5Tswlu5iQB1t5JlApmYjf+SOaOruBXhluDkJ/twP956VHYXFwDO1YGzUsOXpTnrQMNW0ESsd3XFQ/bn+03ZMDC1olr273M0MoLfrMvBeKOFEueJhR2gi+SH5ZEr7gGKGlwi7VzvfVp1QqEjGogs7h3sVeHyjYFaT1bDqH+OU10O+407P7dhZ8u5piTdQ5yCyyO5/SJd8Hs6l0OeAQwWPrKEJIdRARcMIYcMiMtuPijHvrQaFnme5sNhqlsqqiP5prDLRzvDbkm4qF40mWFqKN79PVUZ/C0/o0RdhA6hl1or7bu/kECrrWg1KCFlupsNcwzT/5Pai8IfllzJfZtMMNYPDSh8gMZsoWSI5311um/TMA+w+O1v/yaiakzR2y4GMn/RguUU4BTNXeRXS6vrvaa/LJxdnFG+Z3lOmsYvElu2oeWc2JPol3TIj1yMIvcuAX5U4JZGJjFboCsd+w+OIZhtnSU1staHo9LRKQS8aUX3VwgOUqGI27AbkhXDHNoln/qZcotSEGrxMsM1EIBOHZvyF+g3laGN1z7si/bg4Xm/GDy4XQcWoIcZ51BEfYY4mStjYfATr8Bijxwr0nUNXiPMM3wGjL+st5koKSb9AJwTM8sqXB1MZ584Ciq/sIIdfMAlHWfCoj9QihX7gjMNgangAZwA9ga5K7nZrAQoweBeAcvUEC0snX/++xKNtUrr1Zz2PC7ci9+QlVmKEH7eKy2ASzMSOU5pdhzrd3MLjU8O/6egFA=
 /* For part 2:
    - create a struct for asteroids containing the numbers of asteroids in front (or just the distance??), and the angle:
    (Math.atan2(o.y - this.y, o.x - this.x) * 180) / Math.PI

    dot = x1*x2 + y1*y2      # Dot product between [x1, y1] and [x2, y2]
    det = x1*y2 - y1*x2      # Determinant
    angle = atan2(det, dot)  # atan2(y, x) or atan2(sin, cos)
    https://stackoverflow.com/questions/14066933/direct-way-of-computing-the-clockwise-angle-between-two-vectors/16544330#16544330

    atan2 usually is in the range [-180°,180°]. To get [0°,360°] without a case distinction, one can replace atan2(y,x) with atan2(-y,-x) + 180°

    - Sort first by the count then by the angle */

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define PI 3.14159265358979323846

typedef struct {
    Point2* pos;
    double angle;
    int dist;
    bool destroyed;
} Asteroid;

int astcmp(const void* a, const void* b) {
    Asteroid* ast1 = (Asteroid*)a;
    Asteroid* ast2 = (Asteroid*)b;
    if (ast1->angle == ast2->angle)
        return ast1->dist - ast2->dist;
    return 2*(ast1->angle > ast2->angle) - 1;
}

Point2* parse_input(FILE* input) {
    Point2* asteroids = da_create(sizeof(*asteroids));
    int x = 0; int y = 0;
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (c == '#') {
            da_append(asteroids, ((Point2) { x, y }));
        } else if (c == '\n') {
            x = -1; ++y;
        }
        ++x;
    }
    return asteroids;
}

int main() {
    FILE* input = fopen("data/day_10.txt", "r");
    Point2* candidates = parse_input(input);
    fclose(input);

    int best_count = 0;
    Point2* station = NULL;
    for (size_t i = 0; i < da_length(candidates); ++i) {
        Hset* angles = hs_create(sizeof(double));
        for (size_t j = 0; j < da_length(candidates); ++j) {
            if (i == j)
                continue;
            double angle = atan2(candidates[i].x - candidates[j].x, candidates[j].y - candidates[i].y);
            hs_insert(angles, &angle);
        }
        if ((int)hs_length(*angles) > best_count) {
            best_count = (int)hs_length(*angles);
            station = &candidates[i];
        }
        hs_free(angles);
    }

    size_t asteroids_count = da_length(candidates) - 1;

    // Part 2
    Asteroid* asteroids = malloc(sizeof(*asteroids) * asteroids_count);
    size_t p = 0;
    for (size_t i = 0; i < da_length(candidates); ++i) {
        if (candidates[i].x == station->x && candidates[i].y == station->y)
            continue;
        Point2* pos = &candidates[i];
        asteroids[p].pos = pos;
        asteroids[p].angle = atan2(pos->x - station->x, station->y - pos->y);
        if (asteroids[p].angle < 0) asteroids[p].angle += 360.0;
        asteroids[p].dist = abs(pos->x - station->x) + abs(pos->y - station->y);
        asteroids[p].destroyed = false;
        ++p;
    }

    qsort(asteroids, asteroids_count, sizeof(*asteroids), astcmp);

    double *angle = NULL;
    int cnt = 0;
    size_t i = 0;
    while (cnt < 200) {
        if (i >= asteroids_count) {
            i = 0;
            angle = NULL;
        }
        Asteroid* ast = &asteroids[i];
        if (ast->destroyed || (angle != NULL && ast->angle == *angle)) {
            ++i;
            continue;
        }
        ast->destroyed = true;
        angle = &ast->angle;
        cnt++;
    }
 
    printf("Part 1: %d\n", best_count);
    printf("Part 2: %d\n", asteroids[i].pos->x*100 + asteroids[i].pos->y);
    
    free(asteroids);
    da_free(candidates);
}