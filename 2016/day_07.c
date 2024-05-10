#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 256

void parse_ip(char *ip, char*** supernets, char*** hypernets) {
    char* s = strtok(ip, "[");
    char* h = strtok(NULL, "]");
    while (h != NULL) {
        da_append(*supernets, s);
        da_append(*hypernets, h);
        s = strtok(NULL, "[");
        h = strtok(NULL, "]");
    }
    da_append(*supernets, s);
}

bool has_abba(const char* s) {
    char w[4] = {0};
    while (*s != '\0') {
        memmove(w, w + 1, 3);
        w[3] = *s;
        if (w[0] == w[3] && w[1] == w[2] && w[0] != w[1]) {
            return true;
        }
        ++s;
    }
    return false;
}

bool supports_tls(char **supernets, char** hypernets) {
    for (size_t i = 0; i < da_length(hypernets); ++i) {
        if(has_abba(hypernets[i]))
            return false;
    }
    for (size_t i = 0; i < da_length(supernets); ++i) {
        if(has_abba(supernets[i]))
            return true;
    }
    return false;
}

bool supports_ssl(char **supernets, char** hypernets) {
    for (size_t i = 0; i < da_length(supernets); ++i) {
        char w[3] = {0};

        char *s = supernets[i];
        while (*s != '\0') {
            memmove(w, w + 1, 2);
            w[2] = *s;
            if (w[0] == w[2] && w[0] != w[1]) {
                char bab[4] = {w[1], w[0], w[1], '\0'};
                for (size_t j = 0; j < da_length(hypernets); ++j) {
                    if (strstr(hypernets[j], bab) != NULL)
                        return true;
                }
            }
            ++s;
        }
    }
    return false;
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");

    int tls_cnt = 0;
    int ssl_cnt = 0;
    char buffer[LINE_MAX];
    while(fgets(buffer, sizeof(buffer), input) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';

        char** supernets = da_create(sizeof(char*));
        char** hypernets = da_create(sizeof(char*));
        parse_ip(buffer, &supernets, &hypernets);
        
        tls_cnt += supports_tls(supernets, hypernets);
        ssl_cnt += supports_ssl(supernets, hypernets);

        da_free(supernets);
        da_free(hypernets);
    }
    
    printf("Part 1: %d\n", tls_cnt);
    printf("Part 2: %d\n", ssl_cnt);
    
    fclose(input);
}