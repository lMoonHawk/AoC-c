#include <stdio.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

const char kp1[][3] = { {'1','2','3'},
                        {'4','5','6'},
                        {'7','8','9'}};

const char kp2[][5] = { {' ',' ','1',' ',' '},
                        {' ','2','3','4',' '},
                        {'5','6','7','8','9'},
                        {' ','A','B','C',' '},
                        {' ',' ','D',' ',' '}};

Point2 move_dir(const char c) {
    switch(c) {
        case 'R': return (Point2) {.x=1};
        case 'L': return (Point2) {.x=-1};
        case 'U': return (Point2) {.y=-1};
        case 'D': return (Point2) {.y=1};
    }
    return (Point2) {0};
}

bool is_valid(Point2 pos, int kp_size, const char kp[kp_size][kp_size]) {
    if(pos.x < 0 || pos.x >= kp_size) return false;
    if(pos.y < 0 || pos.y >= kp_size) return false;
    if(kp[pos.y][pos.x] == ' ')       return false;
    return true;
}

int main() {
    FILE* input = fopen("data/day_02.txt", "r");
    
    char *code1 = da_create(sizeof(char));
    Point2 pos1 = {.x=1, .y=1};

    char *code2 = da_create(sizeof(char));
    Point2 pos2 = {.x=0, .y=2};

    char c = fgetc(input);
    while (true) {
        if (c == '\n' || c == EOF) {
            da_append(code1, ((char) {kp1[pos1.y][pos1.x]}));
            da_append(code2, ((char) {kp2[pos2.y][pos2.x]}));

            c = fgetc(input);
            if (c == EOF)
                break;
        }

        Point2 move = move_dir(c);

        Point2 new_pos1 = p2add(pos1, move);
        if(is_valid(new_pos1, 3, kp1))
            pos1 = new_pos1;
        Point2 new_pos2 = p2add(pos2, move);
        if(is_valid(new_pos2, 5, kp2))
            pos2 = new_pos2;
            
        c = fgetc(input);
    }

    printf("Part 1: %s\n", code1);
    printf("Part 2: %s\n", code2);

    da_free(code1);
    da_free(code2);
    fclose(input);
}