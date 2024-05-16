#include <stdio.h>

#define MD5_IMPLEMENTATION
#include "../common/md5.h"
#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define QUEUE_IMPLEMENTATION
#include "../common/queue.h"


#define LINE_MAX 256

typedef struct {
    char c;
    int index;
} Candidate;

// Pass the nibbles as hex characters in out.
void hexdigest(uint8_t hex[16], char out[33]) {
    for (int i = 0; i < 16; ++i) {
        snprintf(out, 3, "%02x", hex[i]);
        out += 2;
    }
}

void md5String_stretch(char in[33], char out[33], int times) {
    uint8_t hashed[16];
    for (int i = 0; i < (times - 1); ++i) {
        md5String(in, hashed);
        hexdigest(hashed, in);
    }
    md5String(in, hashed);
    hexdigest(hashed, out);
}

int get_consecutive(const char* s, Candidate* cand) {
    int cnt = 1;
    int streak = 0;
    char c = -1;

    while (*s != '\0') {
        if (*s == c) {
            ++cnt;
            if ((cnt == 3 && streak == 0) || (cnt == 5 && streak == 3)) {
                streak = cnt;
                cand->c = *s;
                if (streak == 5) {
                    break;
                }
            }
        } else {
            cnt = 1;
        }
        c = *(s++);
    }
    return streak;
}

int get_last_key(const char* salt, int stretch_cnt) {
    Queue candidates = q_create(sizeof(Candidate));

    int index = 1;
    int key_cnt = 0;

    while (1) {

        char in[33];
        char out[33];
        if (snprintf(in, sizeof(in), "%s%d", salt, index) >= (int)sizeof(in)) exit(0);
        md5String_stretch(in, out, stretch_cnt);

        Candidate current = { .c = 0, .index = index };
        int streak = get_consecutive(out, &current);
        if (streak) {
            if (streak == 5) {

                // Remove all elements that cannot be candidates anymore
                 while (q_length(candidates)) {
                    Candidate* prev = (Candidate*)q_peek(candidates);
                    if (prev->index < index - 1000) {
                        q_pop(candidates, NULL);
                    } else {
                        break;
                    }
                }

                // Iterate over the queue to find a key.
                size_t h = candidates->head;
                size_t capa = da_capacity(candidates->arr);
                for (size_t i = 0; i < q_length(candidates); ++i) {
                    Candidate* prev = &((Candidate*)candidates->arr)[h];

                    // previous candidate is a key
                    if (prev->c == current.c) {
                        ++key_cnt;
                        if (key_cnt == 64) {
                            return prev->index;
                        }
                        // Set candiate to 0 so it cannot get referenced again (future delete) 
                        prev->index = 0;
                        prev->c = 0;
                    }
                    h = (h + 1) % capa;
                }

            }
            q_push(candidates, current);
        }
        ++index;
    }

    q_free(candidates);
}

int main() {
    FILE* input = fopen("data/day_14.txt", "r");

    char salt[LINE_MAX];
    if (fgets(salt, sizeof(salt), input) == NULL) exit(0);
    fclose(input);
    salt[strcspn(salt, "\n")] = '\0';

    int last_key1 = get_last_key(salt, 1);
    int last_key2 = get_last_key(salt, 2017);

    printf("Part 1: %d\n", last_key1);
    printf("Part 2: %d\n", last_key2);
}