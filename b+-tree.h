#pragma once

typedef int BOOL;
#define TRUE    1
#define FALSE   0

#define BPTREE_MIN_FACTOR_T     5                   // (See Cormen's Introduction to Algorithms, Chapter 18, B Trees) 
#define BPTREE_MAX_KEYS         ((2 * BPTREE_MIN_FACTOR_T) - 1)
#define BPTREE_MAX_CHILDREN     (BPTREE_MAX_KEYS + 1)

/* forward declaration */
typedef struct bptree_node_s bptree_node;

typedef struct bptree_node_s {
    int keys_count;                                 // πλήθος των κλειδιών που είναι τώρα αποθηκευμένα στο node
    int keys[BPTREE_MAX_KEYS];                      // τα κλειδιά (από αριστερά προς δεξιά κατά αύξουσα σειρά)
    bptree_node* children[BPTREE_MAX_CHILDREN];     // οι θυγατρικοί κόμβοι

    /* code in case node is leaf */
    BOOL is_leaf;
    bptree_node* p_rightmost_leaf;  // δείκτης στο φύλλο δεξιά (εννοείται, είμαι κόμβος-φύλλο)
} bptree_node;

void            bptree_init(bptree_node*);

/*
 * bptree_search
 * 
 * Search for a value (return leaf that contains it)
 * and use a specific node as root.
 * 
 * Returns NULL on failure.
 */
bptree_node*    bptree_search(bptree_node*, int value);

/*
 * bptree_insert
 * 
 * Insert a value to a B+-Tree
 */
BOOL            bptree_insert(bptree_node*, int value);

/*
 * bptree_destroy
 */
void            bptree_destroy(bptree_node* root);