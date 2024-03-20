#include <stdio.h>

#define POS_SIZE 10000

typedef struct {
    int x;
    int y;
} Vec2;

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    char c;
    int x, y;


    Vec2 pos_alone = {};
    Vec2 pos_arr_alone[POS_SIZE];

    Vec2 pos_santa = {};
    Vec2 pos_robot = {};
    Vec2 pos_arr[POS_SIZE];

    int houses_alone = 0;
    int houses = 0;

    int index = 0;
    while ((c = fgetc(input)) != EOF) {
        if (index >= POS_SIZE) {
            fprintf(stderr, "POS_SIZE %d not enough", POS_SIZE);
            return 0;
        }

        Vec2* pos = (index % 2 == 0) ? &pos_santa : &pos_robot;

        if (c == '>') {
            pos->x++;
            pos_alone.x++;
        }
        else if (c == '<') {
            pos->x--;
            pos_alone.x--;
        }
        else if (c == 'v') {
            pos->y++;
            pos_alone.y++;
        }
        else if (c == '^') {
            pos->y--;
            pos_alone.y--;
        }

        pos_arr_alone[index] = pos_alone;
        pos_arr[index] = *pos;

        int unique_alone = 1;
        int unique = 1;
        for (int i = 0; i < index; i++) {
            if (pos_arr_alone[i].x == pos_arr_alone[index].x && pos_arr_alone[i].y == pos_arr_alone[index].y) {
                unique_alone = 0;
            }
            if (pos_arr[i].x == pos_arr[index].x && pos_arr[i].y == pos_arr[index].y) {
                unique = 0;
            }
            if (!unique_alone && !unique) break;
        }
        houses += unique;
        houses_alone += unique_alone;
        index++;
    }

    fclose(input);
    printf("Part 1: %i\n", houses_alone);
    printf("Part 2: %i", houses);
    return 0;
}

