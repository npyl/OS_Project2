#include "stack.h"

#include <stdio.h>
#include <malloc.h>

void stack_init(stack* st) {
    st->root = malloc(sizeof(stack));
    st->root->next = NULL;
    st->root->data = NULL;
    st->size = 1;
}

void* stack_pop(stack* st) {
    stack_node* cursor = st->root;
    stack_node* parent = NULL;
    void* data;

    /* we can't go <= 0; root is sentinel and below zero is irrational! */
    if (st->size == 1)
        return NULL;

    /* find stack tail */
    for (int j = 0; j < st->size; j++)
    {
        parent = cursor;
        cursor = cursor->next;
    }

    /*  */
    data = cursor->data;

    /*  */
    free(cursor);

    /* invalidate tail */
    parent->next = NULL;

    /* reduce size */
    st->size--;

    return data;
}

void stack_push(stack* st, void* data) {
    stack_node* cursor = st->root;
    stack_node* parent = NULL;

    /* find stack tail */
    for (int j = 0; j < st->size; j++)
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
    st->size++;
}

void stack_deinit(stack* st) {
    /* deallocate our stack; should stop when stack_pop() returns NULL; eg. stack is empty */
    while (stack_pop(st))
        ;

    /* don't forget to free the sentinel! */
    free(st->root);
}