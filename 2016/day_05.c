#include <stdio.h>
#include <assert.h>

#define MD5_IMPLEMENTATION
#include "../common/md5.h"

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_05.txt", "r");
    char prefix[LINE_MAX];

    if(fgets(prefix, sizeof(prefix), input) == NULL) exit(0);
    fclose(input);
    prefix[strcspn(prefix, "\n")] = '\0';

    int suffix = 0;
    char pw1[9] = {0};
    int p1 = 0;
    char pw2[9] = {0};
    int p2 = 0;

    while (p1 < 8 || p2 < 8) {
        char door_id[32];
        uint8_t hashed[16];
        
        if(snprintf(door_id, sizeof(door_id), "%s%d", prefix, suffix++) >= (int)sizeof(door_id)) exit(0);
        md5String(door_id, hashed);

        if (hashed[0] + hashed[1] == 0 && hashed[2] <= 0x0F) {
            uint8_t hex6 = hashed[2] & 0x0F;
            if (p1 < 8) {
                char hex[2];
                snprintf(hex, 2, "%x", hex6);
                pw1[p1++] = hex[0];
            }
            if (p2 < 8 && hex6 <= 7 && pw2[hex6] == '\0') {
                char hex[2];
                snprintf(hex, 2, "%x", hashed[3] >> 4);
                pw2[hex6] = hex[0];
                ++p2;
            }
        }
    }
    printf("Part 1: %s\n", pw1);
    printf("Part 2: %s\n", pw2);
}