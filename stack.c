#include "stack.h"

#include <stdio.h>
#include <malloc.h>

void* pop(stack* stack) {
    stack_node* cursor = stack->root;
    stack_node* parent = NULL;

    /* we can't go negative! */
    if (stack->size == 0)
        return NULL;

    /* find stack tail */
    for (int j = 0; j < stack->size; j++)
    {
        parent = cursor;
        cursor = cursor->next;
    }

    /* invalidate tail */
    parent->next = NULL;

    /* reduce size */
    stack->size--;

    return cursor;
}

void push(stack* stack, void* data) {
    stack_node* cursor = stack->root;
    stack_node* parent = NULL;

    /* find stack tail */
    for (int j = 0; j < stack->size; j++)
    {
        parent = cursor;
        cursor = cursor->next;
    }

    /* create new node */
    cursor = malloc(sizeof(stack_node));
    cursor->next = NULL;
    cursor->data = data;

    /* add to tail */
    parent->next = cursor;

    /* increase size */
    stack->size++;
}