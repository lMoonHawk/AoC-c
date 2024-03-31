#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>

#define LOC_MAX_CHAR 50

typedef struct {
    char start[LOC_MAX_CHAR];
    char end[LOC_MAX_CHAR];
    size_t distance;
} Dist;


void swap_c(char** a, char** b) {
    char* temp = *a;
    *a = *b;
    *b = temp;
}


int in_arr_c(const char* key, char* arr[LOC_MAX_CHAR], size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (strcmp(key, arr[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int correct_key(size_t i, size_t j, char** locations, Dist* dists) {
    int cond = 0;
    cond += (strcmp(locations[i], dists[j].start) == 0 && strcmp(locations[i + 1], dists[j].end) == 0);
    cond += (strcmp(locations[i], dists[j].end) == 0 && strcmp(locations[i + 1], dists[j].start) == 0);
    return cond;
}

void append_loc(char* add, size_t nb_locations, size_t* p_capacity, size_t* p_dist_capacity, char*** p_locations, Dist** p_distances) {
    // Appends "add" to locations. Reallocate memory if needed, also reallocating the distance matrix.
    if (nb_locations > *p_capacity) {
        *p_dist_capacity += *p_capacity;
        (*p_capacity)++;
        *p_locations = realloc(*p_locations, *p_capacity * sizeof(char*));
        *p_distances = realloc(*p_distances, *p_dist_capacity * sizeof(Dist));
    }
    (*p_locations)[(nb_locations - 1)] = malloc(LOC_MAX_CHAR * sizeof(char));
    strcpy((*p_locations)[(nb_locations - 1)], add);
}

size_t path_dist(char** locations, Dist* dists, size_t nb_locations) {
    size_t distance = 0;
    for (size_t i = 0; i < (nb_locations - 1); i++) {
        for (size_t j = 0; j < nb_locations * (nb_locations - 1) / 2; j++) {
            if (correct_key(i, j, locations, dists)) {
                distance += dists[j].distance;
                break;
            }
        }
    }
    return distance;
}


void get_perms_bounds(char** locations, size_t start, size_t end, Dist* dists, size_t* min, size_t* max) {
    if (start == end) {
        size_t result = path_dist(locations, dists, end + 1);
        if (result < *min) *min = result;
        if (result > *max) *max = result;
    }
    else {
        for (size_t i = start; i <= end; i++) {
            swap_c(&locations[start], &locations[i]);
            get_perms_bounds(locations, start + 1, end, dists, min, max);
            swap_c(&locations[start], &locations[i]);
        }
    }
}


int main() {
    FILE* input = fopen("data/day_09.txt", "r");
    size_t nb_locations = 0;
    size_t capacity = 2;
    size_t dist_capacity = capacity * (capacity - 1) / 2;

    Dist* distances = malloc(dist_capacity * sizeof(*distances));
    char** locations = malloc(capacity * sizeof(char*));

    char buffer[2*LOC_MAX_CHAR];
    size_t index = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        char* start = strtok(buffer, " ");
        strtok(NULL, " ");
        char* end = strtok(NULL, " ");
        strtok(NULL, " ");
        size_t dist = atoi(strtok(NULL, " "));

        if (!in_arr_c(start, locations, nb_locations)) {
            append_loc(start, ++nb_locations, &capacity, &dist_capacity, &locations, &distances);
        }
        if (!in_arr_c(end, locations, nb_locations)) {
            append_loc(end, ++nb_locations, &capacity, &dist_capacity, &locations, &distances);
        }
        strcpy(distances[index].start, start);
        strcpy(distances[index].end, end);
        distances[index].distance = dist;
        index++;
    }
    fclose(input);

    size_t min = UINT_MAX;
    size_t max = 0;
    get_perms_bounds(locations, 0, nb_locations - 1, distances, &min, &max);

    for (size_t i = 0; i < nb_locations; i++) {
        free(locations[i]);
    }
    free(distances);
    free(locations);

    printf("Part 1: %zu\n", min);
    printf("Part 2: %zu", max);
    return 0;
}
