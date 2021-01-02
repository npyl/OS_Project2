#include <stdio.h>
#include <malloc.h>

#include "b+-tree.h"
#include "stack.h"

//
// Helpers
//

/*
 * split() on a leaf requires knowledge of two things: 
 *      1. the leaf 
 *      2. its parent
 * Since, in order to acquire a leaf, we've already executed
 *      a search in bptree_insert(), it is a smart approach
 *      to have this specific function populate an ancestry
 *      stack with the parents, grandparents, and so on... of
 *      that leaf.
 */
static stack _leaf_ancestry;

bptree_node* get_next_leaf_ancestor()
{
    return stack_pop(&_leaf_ancestry);
}

void create_empty_leaf(bptree_node** new_leaf)
{
    *new_leaf = malloc(sizeof(bptree_node));
    (*new_leaf)->is_leaf            = TRUE;
    (*new_leaf)->keys_count         = 0;
    (*new_leaf)->p_rightmost_leaf   = NULL;
}

void split(bptree_node* leaf, bptree_node* parent, bptree_node** p_new_leaf)
{
    create_empty_leaf(*p_new_leaf);

    
}

BOOL _bptree_insert(int x, bptree_node* leaf, bptree_node* parent, bptree_node** p_new_leaf)
{
    if (leaf->keys_count < BPTREE_MAX_KEYS)
    {
        split(leaf, parent, p_new_leaf);
    }
    else
    {
        /* position to insert key */
        int j = 0;

        bptree_node* cursor = leaf;

        /*
         * There are 3 cases:
         *  1. x must be inserted inside a series of keys (=> j must be found, and then we need to shift right all the rightmost elements)
         *  2. must be inserted after this series (=> j = keys_count which is practically the end of the series +1)
         *  3. no keys have been inserted (=> j = keys_count = 0)
         * 
         * This flag is TRUE for 1. and FALSE for 2. (and 3.).
         */
        BOOL must_shift_position = FALSE;

        /* find where to put kj */
        for (int i = 0; i < cursor->keys_count; i++)
        {
            if (x < cursor->keys[i])
            {
                j = i;
                must_shift_position = TRUE;
                break;
            }
        }

        /* move all keys rightmost to kj one position right */
        if (must_shift_position)
        {
            for (int k = cursor->keys_count; k > j; k--)
            {
                cursor->keys[k] = cursor->keys[k - 1];
            }
        }
        else
        {
            j = cursor->keys_count;
        }

        /* add new key */
        cursor->keys[j] = x;
    }

    return TRUE;
}

//
//========================================================
//

void bptree_init(bptree_node* root)
{
    create_empty_leaf(&root);

    stack_init(&_leaf_ancestry);
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
    printf ("inserting %d\n", x);

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

        /* populate our registry of leaf ancestors */
        stack_push(&_leaf_ancestry, leaf_parent);

        if (x < cursor->keys[0])
        {
            cursor = cursor->children[0];
            continue;
        }
        for (int i = 0; i < cursor->keys_count; i++)
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

    bptree_node* p_new_leaf = NULL;

    // Now use our internal insert to do the heavy lifting!
    BOOL res = _bptree_insert(x, leaf, leaf_parent, &p_new_leaf);
    if (!res)
        return FALSE;

    // increment keys counter
    leaf->keys_count++;

    // insert() required splitting; we should connect old leaf to new leaf!
    if (p_new_leaf != NULL)
        leaf->p_rightmost_leaf = p_new_leaf;

    return TRUE;
}

void bptree_destroy(bptree_node* root) {
    // TODO: fix this causing seg-fault;
    // stack_deinit(&_leaf_ancestry);
}