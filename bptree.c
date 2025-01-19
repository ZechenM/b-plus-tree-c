#include "bptree.h"
#include "assert.h"
#include "math.h"

#define DEBUG_MODE

// it will rearrange all the keys but not ptrs
// ptrs will be set in insert_internal after returning from this function
static int get_insert_index(uint32_t key, BPTreeNode *parent)
{
    int i = 0;
    while (i < parent->num_keys)
    {
        if (key < parent->keys[i])
        {
            // shift keys and ptrs to the right by 1
            // for keys that are greater than the key to be inserted
            for (int j = parent->num_keys; j > i; j--)
            {
                parent->ptrs[j] = parent->ptrs[j - 1];
                parent->keys[j] = parent->keys[j - 1];
            }
            parent->keys[i] = key;
            parent->num_keys++;

            // if i == num_keys - 1, then i will not be incremented
            // we have inserted the second largest key on the node
            break;
        }
        i++;
    }

    // Case 2: key is greater than all keys on the leaf node
    // no shifting for the space is needed
    if (i == parent->num_keys)
    {
        parent->keys[i] = key;
        parent->num_keys++;
        // no new node is created
        // so do need to worry about level field
        // the single linked list on the leaf level
        // should be handled upon creation as well
        // TODO: SINGLE LINKED LIST IS NOT YET IMPLEMENTED
    }

    return i;
}

static void update_level(BPTreeNode *cursor, int level)
{
    printf("Level %d\n", level);
    cursor->level = level;

    if (cursor->is_leaf)
    {
        return;
    }

    for (int i = 0; i < cursor->num_keys + 1; i++)
    {
        update_level(cursor->ptrs[i], level + 1);
    }
}

// cursor is no longer the same cursor as insert()
// cursor is the parent node of the previsour cursor node
static void insert_internal(uint32_t key, BPTreeNode *cursor, BPTreeNode *left_leaf, BPTreeNode *right_leaf)
{
    if (cursor->num_keys < MAX_KEYS)
    {
        int i = get_insert_index(key, cursor);

        cursor->ptrs[i] = (void *)left_leaf;
        cursor->ptrs[i + 1] = (void *)right_leaf;
    }
    else
    {
        BPTreeNode *new_left = (BPTreeNode *)malloc(sizeof(BPTreeNode));
        BPTreeNode *new_right = (BPTreeNode *)malloc(sizeof(BPTreeNode));

        int i = get_insert_index(key, cursor);
        cursor->ptrs[i] = (void *)left_leaf;
        cursor->ptrs[i + 1] = (void *)right_leaf;

        // j right here is the number of pointers your new_left node will have
        int j = (int)(floor((MAX_PTRS + 1) / 2.0));

        new_left->num_keys = j - 1;
        new_left->level = cursor->level;
        new_left->is_leaf = 0;

        // copy the first j pointers (j-1 keys)
        // index order (ptr vs key): [0, j-1] vs [0, j-2]
        int left_index = 0;
        while (left_index < j)
        {
            new_left->ptrs[left_index] = cursor->ptrs[left_index];

            // the last iteration, no key assignment but there will be pointer assignment
            if (left_index != j - 1)
            {
                new_left->keys[left_index] = cursor->keys[left_index];
            }
            left_index++;
        }

        new_right->num_keys = MAX_KEYS + 1 - (j - 1 + 1);
        new_right->level = cursor->level;
        new_right->is_leaf = 0;

        // copy the remaining  pointers
        // keys[j-1] will move up
        // index order (ptr == key): [j, MAX_PTRS] == [j, MAX_KEYS]
        int right_index = 0;
        while (right_index < MAX_PTRS + 1 - j)
        {
            new_right->ptrs[right_index] = cursor->ptrs[right_index + j];

            // the last iteration, no key assignment but there will be pointer assignment
            if (right_index != MAX_PTRS - j)
            {
                new_right->keys[right_index] = cursor->keys[right_index + j];
            }
            right_index++;
        }

        // update the parent node
        if (cursor == root)
        {
            BPTreeNode *new_root = (BPTreeNode *)malloc(sizeof(BPTreeNode));
            new_root->keys[0] = cursor->keys[j - 1];
            new_root->ptrs[0] = (void *)new_left;
            new_root->ptrs[1] = (void *)new_right;
            new_root->num_keys = 1;
            new_root->level = 1;
            new_root->is_leaf = 0;
            new_left->parent = new_root;
            new_right->parent = new_root;
            root = new_root;

            update_level(new_left, 2);
            update_level(new_right, 2);

            // cursor := {2, 3, 4, 5}
            // => {3} -> {{2, 3}, {4, 5}}
            free(cursor);
        }
        else
        {
            insert_internal(cursor->keys[j - 1], cursor->parent, new_left, new_right);
            free(cursor);
        }
    }
}

static void insert_leaf(BPTreeNode *cursor, uint32_t key, uint64_t value)
{
    // insert key-value pair
    // Case 1: key is smaller than some inner key on the leaf node
    int i = 0;
    while (i < cursor->num_keys)
    {
        if (key < cursor->keys[i])
        {
            // shift keys and ptrs to the right by 1
            // for keys that are greater than the key to be inserted  
            for (int j = cursor->num_keys; j > i; j--)
            {
                cursor->ptrs[j] = cursor->ptrs[j - 1];
                cursor->keys[j] = cursor->keys[j - 1];
            }
            cursor->ptrs[i] = (void *)value;
            cursor->keys[i] = key;
            cursor->num_keys++;

            // if i == num_keys - 1, then i will not be incremented
            // we have inserted the second largest key on the node
            break;
        }
        i++;
    }

    // Case 2: key is greater than all keys on the leaf node
    // no shifting for the space is needed
    if (i == cursor->num_keys)
    {
        cursor->ptrs[i] = (void *)value;
        cursor->keys[i] = key;
        cursor->num_keys++;
        // no new node is created
        // so do need to worry about level field
        // the single linked list on the leaf level
        // should be handled upon creation as well
    }
}


static void split(BPTreeNode *cursor, BPTreeNode *parent, uint32_t key, uint64_t value)
{
    insert_leaf(cursor, key, value);
    
    int j = (int)(ceil((MAX_KEYS + 1) / 2.0));

    BPTreeNode *left = (BPTreeNode *)malloc(sizeof(BPTreeNode));
    BPTreeNode *right = (BPTreeNode *)malloc(sizeof(BPTreeNode));

    left->num_keys = j;
    left->level = cursor->level;
    left->is_leaf = 1;
    // copy the first j entries [1, j] == [0, j-1]
    // 1 indexing vs 0 indexing
    int left_index = 0;
    while (left_index < j)
    {
        left->keys[left_index] = cursor->keys[left_index];
        left->ptrs[left_index] = cursor->ptrs[left_index];
        left_index++;
    }
    
    right->num_keys = MAX_KEYS + 1 - j;
    right->level = cursor->level;
    right->is_leaf = 1;
    // copy the remaining entries [j+1, MAX_KEYS + 1] == [j, MAX_KEYS]
    // 1 indexing vs 0 indexing
    int right_index = 0;
    while (right_index < MAX_KEYS + 1 - j)
    {
        right->keys[right_index] = cursor->keys[right_index + j];
        right->ptrs[right_index] = cursor->ptrs[right_index + j];
        right_index++;
    }

    // update the parent node
    if (cursor == root)
    {
        BPTreeNode *new_root = (BPTreeNode *)malloc(sizeof(BPTreeNode));
        new_root->keys[0] = left->keys[j - 1];
        new_root->ptrs[0] = (void *)left;
        new_root->ptrs[1] = (void *)right;
        new_root->num_keys = 1;
        new_root->level = 1;
        new_root->is_leaf = 0;
        left->parent = new_root;
        right->parent = new_root;
        root = new_root;

        update_level(left, 2);
        update_level(right, 2);

        // cursor := {2, 3, 4, 5} 
        // => {3} -> {{2, 3}, {4, 5}}
        free(cursor);
    }
    else
    {
        insert_internal(left->keys[j - 1], parent, left, right);
        free(cursor);
    }
}

void insert(uint32_t key, uint64_t value)
{
    // edge case: key already exists on the tree
    // assert(search(root, key) != 0 && "Key already exists on the tree");

    if (!root)
    {
        root = (BPTreeNode *)malloc(sizeof(BPTreeNode));
        (root)->keys[0] = key;
        (root)->ptrs[0] = (void *)value;
        (root)->is_leaf = 1;
        (root)->num_keys = 1;
        (root)->level = 1;
        (root)->parent = NULL;
    }
    else
    {
        BPTreeNode *cursor = root;
        BPTreeNode *parent;

        while (!cursor->is_leaf)
        {
            parent = cursor;
            for (int i = 0; i < cursor->num_keys; i++)
            {
                if (key <= cursor->keys[i])
                {
                    cursor = cursor->ptrs[i];
                    break;
                }
                
                // if break has not been triggered
                // key is greater than all keys on this node
                if (i == cursor->num_keys - 1)
                {
                    cursor = cursor->ptrs[i + 1];
                    break;
                }
            }
        }

        // cursor will be some leaf node
        if (cursor->num_keys < MAX_KEYS)
        {
            // insertion always happens at the leaf level
            insert_leaf(cursor, key, value);
            cursor->parent = parent;
        }
        else
        {
            split(cursor, parent, key, value);
        }
        
    }
}

uint64_t search(BPTreeNode *node, uint32_t key)
{
#ifdef DEBUG_MODE
    for (int i = 0; i < node->num_keys; i++)
    {
        printf("Level %d has keys[%d] = %d\n", node->level, i, node->keys[i]);
    }
#endif        

    // edge/base case: data cannot be found
    if (!node) return 0;
    if (node->is_leaf)
    {
        for (int i = 0; i < node->num_keys; i++)
        {
            if (key == node->keys[i]) return (uint64_t)node->ptrs[i];
        }

        return 0;
    }

    // recursive steps
    int i = 0;
    while (i < node->num_keys)
    {
        // K_(i-1) < key <= K_i for 1 < i < q
        // X <= K_i for i = 1
        // K_(i-1) < X for i = q
        if (key <= node->keys[i]) break;
        i++;
    }

    return search(node->ptrs[i], key);
}