#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GRID_SIZE 1000

size_t switch_p1(const char* op, short grid[][GRID_SIZE], int start_x, int start_y, int end_x, int end_y) {
    size_t brightness = 0;
    short switch_light;
    for (int i = start_y; i <= end_y; i++) {
        for (int j = start_x; j <= end_x; j++) {
            switch_light = 0;
            if (grid[i][j] == 0 && (strcmp(op, "on") == 0 || strcmp(op, "toggle"))) {
                switch_light = 1;
            }
            else if (grid[i][j] == 1 && (strcmp(op, "off") == 0 || strcmp(op, "toggle"))) {
                switch_light = -1;
            }
            grid[i][j] += switch_light;
            brightness += switch_light;
        }
    }
    return brightness;
}

size_t switch_p2(const char* op, short grid[][GRID_SIZE], int start_x, int start_y, int end_x, int end_y) {
    size_t brightness = 0;
    short switch_light;
    for (int i = start_y; i <= end_y; i++) {
        for (int j = start_x; j <= end_x; j++) {
            switch_light = 0;
            if (strcmp(op, "on") == 0) {
                switch_light += 1;
            }
            else if (strcmp(op, "toggle") == 0) {
                switch_light += 2;
            }
            else {
                switch_light -= (grid[i][j] > 0);
            }
            grid[i][j] += switch_light;
            brightness += switch_light;
        }
    }
    return brightness;
}


int main() {
    char buffer[100];
    FILE* input = fopen("2015/data/day_06.txt", "r");
    short(*grid_p1)[GRID_SIZE] = calloc(sizeof(*grid_p1), GRID_SIZE);
    short(*grid_p2)[GRID_SIZE] = calloc(sizeof(*grid_p2), GRID_SIZE);
    size_t brightness_p1=0;
    size_t brightness_p2=0;

    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        char* op;
        int start_x, start_y, end_x, end_y;

        op = strtok(buffer, " ");
        if (strcmp(op, "turn") == 0) op = strtok(NULL, " ");

        start_x = atoi(strtok(NULL, ","));
        start_y = atoi(strtok(NULL, " "));
        strtok(NULL, " ");
        end_x = atoi(strtok(NULL, ","));
        end_y = atoi(strtok(NULL, ""));

        brightness_p1 += switch_p1(op, grid_p1, start_x, start_y, end_x, end_y);
        brightness_p2 += switch_p2(op, grid_p2, start_x, start_y, end_x, end_y);
    }
    fclose(input);
    printf("Part 1: %zu\n", brightness_p1);
    printf("Part 2: %zu", brightness_p2);
    return 0;
}

