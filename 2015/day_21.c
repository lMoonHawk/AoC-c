#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINE_MAX 256

enum Attr {
    GOLD,
    HP,
    DMG,
    ARM,
    ATTR_CNT,
};

const int wp[][ATTR_CNT] = { {8,0,4,0}, {10,0,5,0}, {25,0,6,0}, {40,0,7,0}, {74 ,0,8,0} };
const int ar[][ATTR_CNT] = { {0,0,0,0}, {13,0,0,1}, {31,0,0,2}, {53,0,0,3}, {75 ,0,0,4}, {102,0,0,5} };
const int rg[][ATTR_CNT] = { {0,0,0,0}, {0 ,0,0,0}, {25,0,1,0}, {50,0,2,0}, {100,0,3,0}, {20 ,0,0,1}, {40,0,0,2}, {80,0,0,3} };

int is_winning(int player[ATTR_CNT], int boss[ATTR_CNT]) {
    int dealt = player[DMG] - boss[ARM];
    if (dealt < 1) dealt = 1;
    int received = boss[DMG] - player[ARM];
    if (received < 1) received = 1;

    div_t player_dr = div(boss[HP], dealt);
    div_t boss_dr = div(player[HP], received);
    return (boss_dr.quot + (boss_dr.rem > 0) >= player_dr.quot + (player_dr.rem > 0));
}


int main() {
    FILE* input = fopen("data/day_21.txt", "r");
    char buffer[LINE_MAX];

    int boss[ATTR_CNT] = { 0 };
    for (int i = 1; i < ATTR_CNT; ++i) {
        fgets(buffer, sizeof(buffer), input);
        boss[i] = atoi(buffer + strcspn(buffer, ":") + 1);
    }

    long min_spent_win = INT_MAX;
    long max_spent_lose = 0;

    int player[ATTR_CNT];
    player[HP] = 100;
    for (size_t w = 0; w < sizeof(wp) / sizeof(wp[0]); ++w) {
        for (size_t a = 0; a < sizeof(ar) / sizeof(ar[0]); ++a) {
            for (size_t r1 = 0; r1 < sizeof(rg) / sizeof(rg[0]) - 1; ++r1) {
                for (size_t r2 = r1 + 1; r2 < sizeof(rg) / sizeof(rg[0]); ++r2) {
                    for (int i = 0; i < ATTR_CNT; ++i) {
                        if (i == HP) continue;
                        player[i] = wp[w][i] + ar[a][i] + rg[r1][i] + rg[r2][i];
                    }
                    int wins = is_winning(player, boss);
                    if (wins && player[GOLD] < min_spent_win)
                        min_spent_win = player[GOLD];
                    else if (!wins && player[GOLD] > max_spent_lose)
                        max_spent_lose = player[GOLD];
                }

            }
        }
    }

    printf("Part 1: %ld\n", min_spent_win);
    printf("Part 2: %ld", max_spent_lose);
}
