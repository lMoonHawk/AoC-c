#include <stdio.h>
#include <stdbool.h>

bool reached_target(int x_vel, int y_vel, int x_lo, int x_hi, int y_lo, int y_hi) {
    int x = 0, y = 0;
    while (true) {
        x += x_vel;
        y += y_vel;
        x_vel = x_vel - 1 > 0 ? x_vel - 1 : 0;
        --y_vel;
        if (x > x_hi || y < y_lo || (x_vel == 0 && x < x_lo))
            return false;
        if (x >= x_lo && x <= x_hi && y >= y_lo && y <= y_hi)
            return true;
    }
}

int main() {
    FILE* input = fopen("data/day_17.txt", "r");
    int x_lo, x_hi, y_lo, y_hi;
    fscanf(input, "target area: x=%d..%d, y=%d..%d", &x_lo, &x_hi, &y_lo, &y_hi);
    fclose(input);
    /*  After y_pos = 0, next step will be:
            y_pos = - v_0 - 1
        <=> v_0 = - y_pos - 1                   */
    int max_vel = -(y_lo + 1);
    // max heigh reached:  v_0 + v_1 + ... 0, i.e. 1 to max_vel
    int highest = max_vel * (max_vel + 1) / 2;

    int count = 0;
    for (int x_vel = 1; x_vel <= x_hi; ++x_vel)
        for (int y_vel = y_lo; y_vel < 1 - y_lo; ++y_vel)
            count += reached_target(x_vel, y_vel, x_lo, x_hi, y_lo, y_hi);

    printf("Part 1: %d\n", highest);
    printf("Part 2: %d\n", count);
}