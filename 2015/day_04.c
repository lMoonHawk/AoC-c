#include <stdio.h>

#define MD5_IMPLEMENTATION
#include "../common/md5.h"


int main() {
    char prefix[16];
    FILE* input = fopen("data/day_04.txt", "r");
    fgets(prefix, sizeof(prefix), input);
    fclose(input);
    prefix[strcspn(prefix, "\n")] = 0;

    int cons_5 = -1;
    for (int suffix = 0; ; suffix++) {
        char buffer[16];
        snprintf(buffer, 16, "%s%d", prefix, suffix);

        uint8_t hashed[16];
        md5String(buffer, hashed);
        // max allowed for 5 consecutive zeros: 0x00 0x00 0x0F 
        if (hashed[0] + hashed[1] == 0 && hashed[2] <= 15) {
            if (cons_5 == -1) cons_5 = suffix;
            if (hashed[2] == 0) {
                printf("Part 1: %d\n", cons_5);
                printf("Part 2: %d", suffix);
                fclose(input);
                return 0;
            }
        }

    }
}

