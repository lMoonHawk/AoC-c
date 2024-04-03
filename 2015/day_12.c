#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define STACK_IMPLEMENTATION
#include "../common/stack.h"

#define BUFFER_MAX 16
// TODO: Use dynamic array or maybe another stack
#define ELEMENTS_MAX 2048

const char banned[] = "red";

typedef struct {
    char paren;
    size_t depth;
    int32_t sum;
    int ignored;
} Element;

void add_char(char c, char* str, size_t size) {
    size_t len = strlen(str);
    assert(len < size);
    str[len] = c;
    str[len + 1] = '\0';
}

// Parses number (increments file pointer)
int parse_num(FILE* f, size_t max_len) {
    char c = fgetc(f);
    char buffer[max_len];
    size_t len = 0;
    while (isdigit(c) || (c == '-' && strlen(buffer) == 0)) {
        assert(len < max_len);
        buffer[len++] = c;
        c = fgetc(f);
    }
    ungetc(c, f);
    buffer[len] = '\0';
    return atoi(buffer);
}

// Parses text until a closing quote is found (increments file pointer)
// Allocates the returned value
char* parse_str(FILE* f, size_t max_len) {

    char c;
    char* buffer = malloc(max_len * sizeof(char));
    size_t len = 0;
    while ((c = fgetc(f)) != '"') {
        assert(len < max_len);
        buffer[len++] = c;
    }
    buffer[len] = '\0';
    return buffer;
}

int main() {
    FILE* input = fopen("data/day_12.txt", "r");

    Stack stack = NULL;
    Element* popped_arr[ELEMENTS_MAX];
    size_t popped_cnt = 0;

    char c;
    uint32_t depth = 0;

    while ((c = fgetc(input)) != EOF) {
        // Found a number
        if (c == '-' || isdigit(c)) {
            ungetc(c, input);
            Element* top = stack_peek(&stack);
            int test = parse_num(input, BUFFER_MAX);
            top->sum += test;
            assert(top->sum < 4294967290);

        }
        // Found a string
        else if (c == '"') {
            char* str = parse_str(input, BUFFER_MAX);
            if (strcmp(banned, str) == 0) {
                Element* top = stack_peek(&stack);
                if (top->paren == '{') {
                    top->ignored = 1;

                    for (ssize_t i = popped_cnt - 1; i >= 0; i--) {
                        Element* node = popped_arr[i];
                        if (node->depth <= top->depth) break;
                        node->ignored = 1;
                    }
                }
            }
            free(str);
        }

        if (c == '[' || c == '{') {
            Element* tmp = calloc(1, sizeof(Element));

            int ignore = 0;
            if (stack != NULL) {
                Element* top = stack_peek(&stack);
                ignore = (top->ignored && top->depth < depth); 
            }
            *tmp = (Element){ .paren = c, .depth = depth, .ignored = ignore };
            stack_push(&stack, tmp);
            depth++;
        }
        else if (c == ']' || c == '}') {
            popped_arr[popped_cnt++] = stack_pop(&stack);
            depth--;
        }
    }

    assert(stack == NULL);
    int32_t total = 0;
    int32_t total_wo_banned = 0;
    int32_t amt;

    for (size_t i = 0; i < popped_cnt; i++) {
        amt = popped_arr[i]->sum;
        
        total += amt;
        if (!popped_arr[i]->ignored) total_wo_banned += amt;
        free(popped_arr[i]);
    }
    printf("Part 1: %d\n", total);
    printf("Part 2: %d", total_wo_banned);

    return 0;
}