#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

typedef struct {
    int id, x, y, w, h;
} Claim;

bool is_claim_unique(Claim claim, int fabric[1000][1000], int* unique_id) {
    for (int x = claim.x; x < claim.x + claim.w; ++x) {
        for (int y = claim.y; y < claim.y + claim.h; ++y) {
            if (fabric[y][x] > 1)
                return false;
        }
    }
    *unique_id = claim.id;
    return true;
}

Claim* parse_input(FILE* input) {
    Claim* claims = da_create(sizeof(*claims));
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        Claim claim = { 0 };
        claim.id = atoi(buffer + 1);
        strtok(buffer, " ");
        strtok(NULL, " ");
        claim.x = atoi(strtok(NULL, ","));
        claim.y = atoi(strtok(NULL, ":"));
        claim.w = atoi(strtok(NULL, "x"));
        claim.h = atoi(strtok(NULL, "\r\n"));
        da_append(claims, claim);
    }
    return claims;
}

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    Claim* claims = parse_input(input);
    fclose(input);

    int(*fabric)[1000] = calloc(1000, sizeof(*fabric));

    int overlap = 0;
    for (size_t i = 0; i < da_length(claims); ++i) {
        for (int x = claims[i].x; x < claims[i].x + claims[i].w; ++x) {
            for (int y = claims[i].y; y < claims[i].y + claims[i].h; ++y) {
                overlap += fabric[y][x] == 1;
                ++fabric[y][x];
            }
        }
    }
    int unique_id = -1;
    for (size_t i = 0; i < da_length(claims) && !is_claim_unique(claims[i], fabric, &unique_id); ++i);
            
    da_free(claims);
    free(fabric);

    printf("Part 1: %d\n", overlap);
    printf("Part 2: %d\n", unique_id);
}