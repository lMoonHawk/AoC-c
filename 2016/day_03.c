#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_03.txt", "r");
    
    int count_p1 = 0;
    int count_p2 = 0;

    int sides[3][3];
    int sides_cnt = 0;

    char buffer[LINE_MAX];
    while(fgets(buffer, sizeof(buffer), input) != NULL) {
        int a = atoi(strtok(buffer, " "));
        int b = atoi(strtok(NULL, " "));
        int c = atoi(strtok(NULL, " "));
        if (a+b>c && b+c>a && a+c>b) 
            ++count_p1;

        sides[0][sides_cnt] = a;
        sides[1][sides_cnt] = b;
        sides[2][sides_cnt] = c;
        ++sides_cnt;
        if (sides_cnt == 3) {
            for (int i = 0; i < 3; ++i) {
                if (sides[i][0] + sides[i][1] > sides[i][2] && 
                    sides[i][0] + sides[i][2] > sides[i][1] && 
                    sides[i][1] + sides[i][2] > sides[i][0]) 
                        ++count_p2;
            }
            sides_cnt = 0;
        }
    }
    printf("Part 1: %d\n", count_p1);
    printf("Part 2: %d\n", count_p2);
    fclose(input);
}