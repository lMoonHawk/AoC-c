#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define MAX_DIGITS 16

typedef struct {
    char ch;
    int read;
} Cursor;

void get_next(FILE* input, Cursor* c) {
    c->ch = fgetc(input);
    c->read++;
}

// Read until marker starts / EOF / max size allowed for the call.
// Returns true if a marker was found.
bool parse_to_marker(FILE* input, Cursor* cur, int max_size, int64_t* len) {
    while (1) {
        get_next(input, cur);
        // Character not part of the decompression lenght
        if (cur->ch == '\n' || cur->ch == '\r' || cur->ch == EOF) {
            return false;
        } else if (cur->ch == '(') {
            return true;
        }
        // Character part of the decompression lenght
        ++(*len);
        if (cur->read == max_size) {
            return false;
        }
    }
}

int parse_num(FILE* input, Cursor* cur, char until) {
    int out = 0;
    get_next(input, cur);
    while (cur->ch != until && cur->ch != EOF) {
        out = out * 10 + (cur->ch - '0');
        get_next(input, cur);
    }
    assert(cur->ch != EOF);
    return out;
}

int64_t get_decomp_len(FILE* input, int max_size, bool recurse) {
    int64_t len = 0;
    Cursor cur = { 1, 0 };
    while (cur.read < max_size) {
        // Read up to marker
        if (!parse_to_marker(input, &cur, max_size, &len)) {
            return len;
        }
        // Parse marker
        int size = parse_num(input, &cur, 'x');
        int rep = parse_num(input, &cur, ')');

        if (recurse) {
            len += rep * get_decomp_len(input, size, true);
            cur.read += size;
        } else {
            // Consume size
            fseek(input, size, SEEK_CUR);
            len += rep * size;
        }
    }
    return len;
}

int main() {
    FILE* input = fopen("data/day_09.txt", "r");

    int64_t len_v1 = get_decomp_len(input, INT_MAX, false);
    fseek(input, 0, SEEK_SET);
    int64_t len_v2 = get_decomp_len(input, INT_MAX, true);

    printf("Part 1: %ld\n", len_v1);
    printf("Part 2: %ld\n", len_v2);

    fclose(input);
}