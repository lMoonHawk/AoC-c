#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define STACK_IMPLEMENTATION
#include "../common/stack.h"

#define BUFFER_MAX 16

const char banned[] = "red";

typedef struct {
    char paren;
    size_t depth;
    int32_t sum;
    int ignored;
} Element;

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

    Stack in_stack = NULL;
    Stack out_stack = NULL;

    char c;
    uint32_t depth = 0;

    while ((c = fgetc(input)) != EOF) {
        // Found a number
        if (c == '-' || isdigit(c)) {
            ungetc(c, input);
            Element* top = stack_peek(&in_stack);
            int test = parse_num(input, BUFFER_MAX);
            top->sum += test;

        }
        // Found a string
        else if (c == '"') {
            char* str = parse_str(input, BUFFER_MAX);
            if (strcmp(banned, str) == 0) {
                Element* top = stack_peek(&in_stack);
                if (top->paren == '{') {
                    top->ignored = 1;

                    Node* item = out_stack;
                    while (item) {
                        Element* node = item->value;
                        if (node->depth <= top->depth) break;
                        node->ignored = 1;
                        item = item->next;
                    }
                }
            }
            free(str);
        }

        if (c == '[' || c == '{') {
            Element* tmp = calloc(1, sizeof(Element));

            int ignore = 0;
            if (in_stack != NULL) {
                Element* top = stack_peek(&in_stack);
                ignore = (top->ignored && top->depth < depth);
            }
            *tmp = (Element){ .paren = c, .depth = depth, .ignored = ignore };
            stack_push(&in_stack, tmp);
            depth++;
        }
        else if (c == ']' || c == '}') {
            stack_push(&out_stack, stack_pop(&in_stack));
            depth--;
        }
    }

    assert(in_stack == NULL);
    int32_t total = 0;
    int32_t total_wo_banned = 0;

    while (out_stack != NULL) {
        Element* node = stack_pop(&out_stack);

        total += node->sum;
        if (!node->ignored) total_wo_banned += node->sum;
        free(node);
    }
    printf("Part 1: %d\n", total);
    printf("Part 2: %d", total_wo_banned);

    return 0;
}