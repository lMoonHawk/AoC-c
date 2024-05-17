#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define LINE_MAX 256

char* get_checksum(const char* str, size_t disk_space) {
    // Worse case is (n-1)*2+1 + null
    char* out = calloc((2 * disk_space), sizeof(char));
    strcpy(out, str);

    size_t n = strlen(str);
    while (n < disk_space) {
        size_t n2 = n * 2;
        for (size_t i = 0; i < n; ++i) {
            out[n2 - i] = (~(out[i] - '0')) + '0';
        }
        out[n] = '0';
        n = n * 2 + 1;
    }

    while (disk_space % 2 == 0) {
        size_t i = 0, j = 0;
        while (i < disk_space) {
            out[j++] = (out[i] == out[i + 1]) + '0';
            i += 2;
        }
        disk_space /= 2;
    }
    out[disk_space] = 0;
    return out;
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    if (!fread(buffer, 1, LINE_MAX, input)) exit(0);
    fclose(input);

    char* cs1 = get_checksum(buffer, 272);
    char* cs2 = get_checksum(buffer, 35651584);

    printf("Part 1: %s\n", cs1);
    printf("Part 2: %s\n", cs2);

    free(cs1);
    free(cs2);
}