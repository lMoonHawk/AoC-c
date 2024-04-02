#ifndef STACK_H
#define STACK_H

#include <stdio.h>

typedef struct Node {
    void* value;
    struct Node* next;
} Node;

typedef Node* Stack;

void stack_push(Stack* stack, void* in);
void* stack_pop(Stack* stack);

#endif // STACK_H

#ifdef STACK_IMPLEMENTATION

#include <stdlib.h>
#include <assert.h>

void stack_push(Stack* stack, void* in) {
    Node* new_node = malloc(sizeof(Node));
    assert(new_node != NULL);
    new_node->value = in;
    new_node->next = *stack;
    *stack = new_node;
}

void* stack_pop(Stack* stack) {
    assert(*stack != NULL);
    void* popped_val = (*stack)->value;
    Node* del_node = *stack;
    *stack = (*stack)->next;
    free(del_node);

    return popped_val;
}

#endif // STACK_IMPLEMENTATION