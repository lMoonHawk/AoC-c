#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define HMAP_IMPLEMENTATION
#include "../common/hmap.h"

#define SIZE 5

int32_t parse_input(FILE* input) {
    int32_t bugs = 0;
    char c;
    int i = 0;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n' || c == '\r')
            continue;
        if (c == '#')
            bugs |= 1 << i;
        ++i;
    }
    return bugs;
}

bool is_bug(int32_t bugs, int i, int j) {
    if (i < 0 || i >= SIZE || j < 0 || j >= SIZE)
        return false;
    int shift = i * SIZE + j;
    return (bugs >> shift) & 1;
}

int is_bug_rec(int32_t* bugs, int i, int j, int ni, int nj, int zi) {
    int result = 0;
    if (ni == -1) {
        result = is_bug(bugs[zi - 1], 1, 2);
    } else if (ni == 5) {
        result = is_bug(bugs[zi - 1], 3, 2);
    } else if (nj == -1) {
        result = is_bug(bugs[zi - 1], 2, 1);
    } else if (nj == 5) {
        result = is_bug(bugs[zi - 1], 2, 3);
    } else if (ni == 2 && nj == 2) {
        if (i == 2 && j == 1) {
            for (int k = 0; k < SIZE; ++k)
                result += is_bug(bugs[zi + 1], k, 0);
        } else if (i == 3 && j == 2) {
            for (int k = 0; k < SIZE; ++k)
                result += is_bug(bugs[zi + 1], 4, k);
        } else if (i == 2 && j == 3) {
            for (int k = 0; k < SIZE; ++k)
                result += is_bug(bugs[zi + 1], k, 4);
        } else if (i == 1 && j == 2) {
            for (int k = 0; k < SIZE; ++k)
                result += is_bug(bugs[zi + 1], 0, k);
        }
    } else {
        result = is_bug(bugs[zi], ni, nj);
    }
    return result;
}

int count_neighbors(int32_t bugs, int i, int j) {
    return is_bug(bugs, i - 1, j) + is_bug(bugs, i + 1, j) +
        is_bug(bugs, i, j - 1) + is_bug(bugs, i, j + 1);
}

int count_neighbors_rec(int32_t* bugs, int i, int j, int zi) {
    int result = 0;
    int dx[4] = { -1, 0, 1, 0 };
    int dy[4] = { 0, 1, 0, -1 };
    for (int k = 0; k < 4; ++k)
        result += is_bug_rec(bugs, i, j, i + dx[k], j + dy[k], zi);
    return result;
}

int32_t compute_next_bugs(int32_t bugs) {
    int32_t next_bugs = bugs;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            bool bug = is_bug(bugs, i, j);
            int neigh = count_neighbors(bugs, i, j);
            int shift = i * SIZE + j;
            if (!bug && neigh >= 1 && neigh <= 2)
                next_bugs |= 1 << shift;
            else if (bug && neigh != 1)
                next_bugs &= ~(1 << shift);
        }
    }
    return next_bugs;
}

int main() {
    FILE* input = fopen("data/day_24.txt", "r");
    int32_t bugs_init = parse_input(input);
    fclose(input);

    int32_t bugs_p1 = bugs_init;
    Hset* seen = hs_create(sizeof(int32_t));
    hs_insert(seen, &bugs_p1);
    for (int i = 0; ; ++i) {
        bugs_p1 = compute_next_bugs(bugs_p1);
        if (hs_contains(*seen, &bugs_p1))
            break;
        hs_insert(seen, &bugs_p1);
    }
    hs_free(seen);


    // Max number of layers with bugs after 200 iterations is 401 (200 negative, 200 positive + 0th).
    // Indexes denote the z coordinate, 0 = -200, 200 = 0, 400 = 200
    int32_t* bugs_p2 = calloc(401, sizeof(int32_t));
    int32_t* next_bugs_p2 = calloc(401, sizeof(int32_t));

    bugs_p2[200] = bugs_init;
    next_bugs_p2[200] = bugs_init;
    int bugs_count = 0;
    int max_z = 0;
    int next_max_z = 0;
    for (int k = 0; k < 200; ++k) {
        bugs_count = 0;
        for (int z = -max_z - 1; z <= max_z + 1; ++z) {
            int zi = z + 200;
            assert(zi >= 0 && zi <= 400);

            for (int i = 0; i < SIZE; ++i) {
                for (int j = 0; j < SIZE; ++j) {
                    if (i == 2 && j == 2)
                        continue;
                    bool bug = is_bug(bugs_p2[zi], i, j);
                    int neigh = count_neighbors_rec(bugs_p2, i, j, zi);
                    int shift = i * SIZE + j;
                    if (!bug && neigh >= 1 && neigh <= 2) {
                        next_bugs_p2[zi] |= 1 << shift;
                        if (z == abs(max_z + 1))
                            next_max_z = z;
                    } else if (bug && neigh != 1) {
                        next_bugs_p2[zi] &= ~(1 << shift);
                    }
                    if (is_bug(next_bugs_p2[zi], i, j))
                        ++bugs_count;
                }
            }
        }
        max_z = next_max_z;
        memcpy(bugs_p2, next_bugs_p2, 401 * sizeof(int32_t));
    }
    free(bugs_p2);
    free(next_bugs_p2);

    printf("Part 1: %d\n", bugs_p1);
    printf("Part 2: %d\n", bugs_count);

}

