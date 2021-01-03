#include <math.h>
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
static int _ancestors_read_count = 0;

bptree_node* get_next_leaf_ancestor()
{
    _ancestors_read_count++;
    return stack_read_top(&_leaf_ancestry);
}

void revert_ancestry_registry()
{
    for (int i = 0; i < _ancestors_read_count; i++)
        stack_read_top_revert(&_leaf_ancestry);
}

void create_empty_leaf(bptree_node** new_leaf)
{
    *new_leaf = malloc(sizeof(bptree_node));
    (*new_leaf)->is_leaf            = TRUE;
    (*new_leaf)->keys_count         = 0;
    (*new_leaf)->p_rightmost_leaf   = NULL;
}
void create_empty_node(bptree_node** new_node)
{
    *new_node = malloc(sizeof(bptree_node));
    (*new_node)->is_leaf            = FALSE;
    (*new_node)->keys_count         = 0;
    (*new_node)->p_rightmost_leaf   = NULL;
}

/* forward declaration */
void _bptree_insert(int x, bptree_node* node, bptree_node* parent);

/*
 * split()
 *
 * split() handles *both* creating new leaf/node *and* 
 *      inserting appropriate values. It is used by
 *      _bptree_insert() and therefore can be called
 *      multiple times if needed.
 */
void split(int x, bptree_node* node, bptree_node* parent, bptree_node** p_new_leaf)
{
    bptree_node* tree_root = stack_root(&_leaf_ancestry);

    if (node == tree_root)
    {
        bptree_node* new_root = NULL;

        /* find key to insert to parent */
        int k = floor(node->keys_count / 2);
        int split_value = node->keys[k];

        /* indexers for inserting appropriate keys to new_root and new node/leaf */
        int j = 0;
        int i = 0;

        /* check if root is the only node (<=> leaf) */
        if (tree_root->children[0] == NULL)
        {
            create_empty_leaf(p_new_leaf);
            create_empty_node(&new_root);

            /* 
             * root is leaf thus, everything inside it is important; 
             * make sure to copy it inside new_leaf 
             */
            i = k + 1;

            /* remove split_value from old root */
            tree_root->keys[k] = 0;
            tree_root->keys_count--;

        }
        else
        {
            create_empty_node(p_new_leaf);
            create_empty_node(&new_root);

            /* 
             * contrary to above case, root's keys are merely guides for
             * tree traversal; we therefore don't need to copy split_value
             * inside new_node (called new_leaf for convenience inside code). 
             */
            i = k;
        }

        /* insert split_value to new root */
        _bptree_insert(split_value, new_root, NULL);

        /* move keys */
        for (; i < tree_root->keys_count; i++)
        {
            (*p_new_leaf)->keys[j] = tree_root->keys[i];
            (*p_new_leaf)->keys_count++;

            tree_root->keys[i] = 0;
            tree_root->keys_count--;

            j++;
        }

        /* insert x into the appropriate child */
        if (x < split_value)
            _bptree_insert(x, tree_root, NULL);
        else if (x >= split_value)
            _bptree_insert(x, *p_new_leaf, NULL);

        /* move children (if there are any) */
        if (tree_root->children[0] != NULL)
        {
            j = 0;
            i = k + 1;

            for (; i < tree_root->keys_count; i++)
            {
                (*p_new_leaf)->children[j] = tree_root->children[i];
                tree_root->children[i] = NULL;

                j++;
            }
        }

        new_root->children[0] = tree_root;
        new_root->children[1] = *p_new_leaf;
    }
    else if (node->is_leaf)
    {
        create_empty_leaf(p_new_leaf);

        /* find key to insert to parent */
        int k = floor(node->keys_count / 2);
        int split_value = node->keys[k];

        int j = 0;
        for (int i = k; i < node->keys_count; i++)
        {
            (*p_new_leaf)->keys[j] = node->keys[i];
            (*p_new_leaf)->keys_count++;

            node->keys[i] = 0;
            node->keys_count--;

            j++;
        }

        /* insert x to new_leaf */
        if (x >= split_value)
            _bptree_insert(x, *p_new_leaf, parent);
        else if (x < split_value)
            _bptree_insert(x, node, parent);

        /* get parent's parent; he exists in the ancestry registry */
        get_next_leaf_ancestor();
        bptree_node* grandparent = get_next_leaf_ancestor();

        /* now insert split_value to parent */
        _bptree_insert(split_value, parent, grandparent);
    }
    else
    {
        bptree_node* new_node;

        create_empty_node(&new_node);

        printf("Internal node splitting!\n");
    }
}

void _bptree_insert(int x, bptree_node* node, bptree_node* parent)
{
    if (node->keys_count == BPTREE_MAX_KEYS)
    {
        if (parent == NULL)
        {
            printf("Error(_bptree_insert): parent shouldn't be NULL.\n");
            return;
        }

        bptree_node* p_new_node = NULL;

        /* leaf is full; call split to handle both adding x and organising the leaves */
        split(x, node, parent, &p_new_node);
        
        /* we should connect old leaf to new leaf! */
        if (node->is_leaf)
            node->p_rightmost_leaf = p_new_node;
    }
    else
    {
        /* position to insert key */
        int i = 0;

        bptree_node* cursor = node;

        /* find where to put ki */
        if (x < cursor->keys[0])
            i = 0;
        while (x > cursor->keys[i] && i < cursor->keys_count)
            i++;

        /* move every key rightmost to ki to the right */
        for (int k = cursor->keys_count; k > i; k--)
        {
            cursor->keys[k] = cursor->keys[k - 1];
        }

        /* add new key */
        cursor->keys[i] = x;
    }
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
        int i = 0;
        while ((i < node->keys_count) && (value > node->keys[i]))
            i++;
        bptree_search(node->children[i], value);
    }
    else if (value >= node->keys[node->keys_count - 1])
    {
        return bptree_search(node->children[node->keys_count], value);
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
                cursor = cursor->children[i + 1];
                break;
            }
        }
    }

    // We have found our leaf
    leaf = cursor;

    // Now use our internal insert to do the heavy lifting!
    _bptree_insert(x, leaf, leaf_parent);

    // increment keys counter
    leaf->keys_count++;

    return TRUE;
}

void bptree_destroy(bptree_node* root) {
    // TODO: fix this causing seg-fault;
    // stack_deinit(&_leaf_ancestry);
}