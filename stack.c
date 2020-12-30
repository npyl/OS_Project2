#include "stack.h"

#include <stdio.h>
#include <malloc.h>

void stack_init(stack* stack) {
    stack->root = malloc(sizeof(stack));
    stack->root->next = NULL;
    stack->root->data = NULL;
    stack->size = 1;
}

void* stack_pop(stack* stack) {
    stack_node* cursor = stack->root;
    stack_node* parent = NULL;

    /* we can't go <= 0; root is sentinel and below zero is irrational! */
    if (stack->size == 1)
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

void stack_push(stack* stack, void* data) {
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

void stack_deinit(stack* s) {
    /* deallocate our stack; should stop when stack_pop() returns NULL; eg. stack is empty */
    while (stack_pop(s))
        ;

    /* don't forget to free the sentinel! */
    free(s->root);
}