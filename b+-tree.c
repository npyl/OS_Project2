#include <stdio.h>

#include "b+-tree.h"

bptree_node*    bptree_search(bptree_node* node, int value)
{
    int i = 1;

    /*
     * We reached to a leaf. This means 2 things:
     * - We found the value we were looking for
     * - and this happened in a previous recursion; we were called
     *      and the node passed is the leaf we found the `value`.
     * Make sure we are a leaf before returning ourselves.
     */
    if (node->is_leaf)
        return node;

    /*
     * We are not a leaf; the search must continue!
     * Find correct (ki, ki+1) range (where ki, ki+1 keys)
     */
    if (value <= node->keys[0])
        return bptree_search(&(node->children[0]), value);

    else if ((value > node->keys[0]) && (value < node->keys[node->keys_count - 1]))
    {
        int i = 1;
        while ((i < node->keys_count) && (value > node->keys[i]))
            i++;
        bptree_search(&(node->children[i]), value);
    }
    else if (value >= node->keys)
    {
        return bptree_search(&(node->children[node->keys_count - 1]), value);
    }

    // else:
    return NULL;
}