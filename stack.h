#pragma once

/* forward declaration */
typedef struct stack_node_s stack_node;

typedef struct stack_node_s {
    stack_node* next;
    void* data;
} stack_node;

typedef struct stack_s {
    stack_node* root;
    int size;
} stack;

void    stack_init(stack*);
void*   stack_pop(stack*);
void    stack_push(stack*, void*);
void    stack_deinit(stack*);