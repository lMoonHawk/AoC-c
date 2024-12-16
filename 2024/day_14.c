#include <stdio.h>

#define GEOM_IMPLEMENTATION
#include "../common/geom.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define _LINE_MAX 256
#define HEIGHT 103
#define WIDTH 101

typedef struct {
    Point2 p, v;
} Robot;

static inline int mod(int a, int b) {
    int result = a % b;
    return result >= 0 ? result : result + b;
}

bool has_cluster(bool(*placement)[WIDTH]) {
    for (int i = 0; i < HEIGHT - 3; i += 3) {
        for (int j = 0; j < WIDTH - 3; j += 3) {
            int count = 0;
            for (int k = 0; k < 3; ++k) {
                for (int l = 0; l < 3; ++l) {
                    count += placement[i + k][j + l];
                }
            }
            if (count == 9)
                return true;
        }
    }
    return false;
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");

    char buffer[_LINE_MAX] = { 0 };
    int x_mid = (WIDTH - 1) / 2;
    int y_mid = (HEIGHT - 1) / 2;
    int quadrants[4] = { 0 };

    Robot* robots = da_create(sizeof(Robot));
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Point2 p, v;
        sscanf(buffer, "p=%d,%d v=%d,%d", &p.x, &p.y, &v.x, &v.y);
        da_append(robots, ((Robot) {.p = p, .v = v }));
        int x = mod(p.x + v.x * 100, WIDTH);
        int y = mod(p.y + v.y * 100, HEIGHT);
        if (x != x_mid && y != y_mid)
            ++quadrants[(x > x_mid) + 2 * (y > y_mid)];
    }
    fclose(input);

    int sec = 0;
    bool(*placement)[WIDTH] = malloc(WIDTH * HEIGHT * sizeof(bool));

    while (1) {
        memset(placement, false, WIDTH * HEIGHT);
        sec++;
        for (size_t i = 0; i < da_length(robots); ++i) {
            robots[i].p.x = mod(robots[i].p.x + robots[i].v.x, WIDTH);
            robots[i].p.y = mod(robots[i].p.y + robots[i].v.y, HEIGHT);
            placement[robots[i].p.y][robots[i].p.x] = true;
        }
        if (has_cluster(placement))
            break;
    }
    free(placement);
    da_free(robots);

    printf("Part 1: %d\n", quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3]);
    printf("Part 2: %d\n", sec);
}