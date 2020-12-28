typedef int BOOL;
#define TRUE    1
#define FALSE   0

typedef struct bptree_node_s {
    int keys_count;                 // πλήθος των κλειδιών που είναι τώρα αποθηκευμένα στο node
    int* keys;                      // τα κλειδιά (από αριστερά προς δεξιά κατά αύξουσα σειρά)
    bptree_node* children;          // οι θυγατρικοί κόμβοι

    /* code in case node is leaf */
    BOOL is_leaf;
    bptree_node* p_rightmost_leaf;  // δείκτης στο φύλλο δεξιά (εννοείται, είμαι κόμβος-φύλλο)
} bptree_node;

int             bptree_get_next_leaf_data();

/*
 * bptree_search
 * 
 * Search for a value (will be found in a leaf, if existent)
 * and use a specific node as root.
 */
bptree_node*    bptree_search(bptree_node*, int value);