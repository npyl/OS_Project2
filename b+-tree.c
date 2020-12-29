#include <stdio.h>
#include <malloc.h>

#include "b+-tree.h"

//
// Helpers
//

void create_empty_leaf(bptree_node** new_leaf)
{
    *new_leaf = malloc(sizeof(bptree_node));

    (*new_leaf)->is_leaf            = TRUE;
    (*new_leaf)->keys_count         = 0;
    (*new_leaf)->p_rightmost_leaf   = NULL;
}

//
//========================================================
//

void bptree_init(bptree_node* root)
{
    create_empty_leaf(&root);
}

bptree_node* bptree_search(bptree_node* node, int value)
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
        return bptree_search(node->children[0], value);

    else if ((value > node->keys[0]) && (value < node->keys[node->keys_count - 1]))
    {
        int i = 1;
        while ((i < node->keys_count) && (value > node->keys[i]))
            i++;
        bptree_search(node->children[i], value);
    }
    else if (value >= node->keys[node->keys_count - 1])
    {
        return bptree_search(node->children[node->keys_count - 1], value);
    }

    // else:
    return NULL;
}

/*
 * bptree_insert()
 * 
 * First of all, we suppose that the tree has been initialised (has a non-null root)
 */
BOOL bptree_insert(bptree_node* tree, int x)
{
    bptree_node* leaf;
    bptree_node* leaf_parent;

    bptree_node* cursor = tree;

    /*
     * Find appropriate leaf to add value `x`.
     * Also, take note of its parent.
     */
    while (!cursor->is_leaf)
    {
        leaf_parent = cursor;

        for (int i = 0; i = cursor->keys_count; i++)
        {
            if (x > cursor->keys[i]) 
            {
                cursor = cursor->children[i];
                break;
            }

            // guard for last element
            if (i == (cursor->keys_count - 1))
                cursor = cursor->children[cursor->keys_count];
        }
    }

    // We have found our leaf
    leaf = cursor;

    return FALSE;
}

void bptree_destroy(bptree_node* root) {}