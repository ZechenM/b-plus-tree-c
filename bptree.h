#ifndef BPTREE_H
#define BPTREE_H

#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "time.h"

// typedef unsigned int uint32_t;
// typedef unsigned long int uint64_t;

#define MAX_PTRS 4
#define MAX_KEYS MAX_PTRS - 1

// struct node *parent; do I need this?
typedef struct BPTreeNode {
    // 0 + 4 * (3+1) = 16 bytes
    uint32_t keys[MAX_KEYS + 1];
    // 16 + 8 * (4+1) = 56 bytes
    void *ptrs[MAX_PTRS + 1];
    // 56 + 4 = 60 bytes
    // the actual number of keys on the node
    uint32_t num_keys;
    // TODO: RECALCULATE THE SIZE OF THE STRUCTURE
    // 52 + 2 = 54 bytes
    uint16_t level;
    // 54 + 2 = 56 bytes
    uint16_t is_leaf;
    // 56 + 8 = 64 bytes
    void *parent;
} __attribute__((packed)) BPTreeNode;

typedef struct OverflowedNode
{
    uint32_t keys[MAX_KEYS + 1];
    void *ptrs[MAX_PTRS];
    uint64_t num_keys;
    uint64_t level;
    uint32_t is_leaf;
} __attribute__((packed)) OverflowedNode;

// the root node of the B+ tree
BPTreeNode *root;

/*
* insert a key-value pair into B+ tree 
* that supports SELF-BALANCING
* @param root: the root of the B+ tree
* @param key: the key to insert
* @param value: the value accessed by a data pointer on the leaf
*/
void insert(uint32_t key, uint64_t value);

/*
* search for a key in the B+ tree
* @param root: the root of the B+ tree
* @param key: the key to search for
* @return: the value associated with the key
*/
uint64_t search(BPTreeNode *node, uint32_t key);


#endif