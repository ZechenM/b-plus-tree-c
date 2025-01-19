#include "bptree.h"
#include "assert.h"

#define DEBUG_MODE

uint32_t keys_arr[20] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71};
uint64_t values_arr[20]; 

static void initialize_values_arr()
{
    for (int i = 0; i < 20; i++)
    {
        values_arr[i] = keys_arr[i] + 100;
    }
}

static void unit_test_leaf_split()
{
    for (int i = 0; i < 4; i++)
    {
        insert(keys_arr[i], values_arr[i]);
    }

    search(root, 3);
}


static void unit_test_root_split()
{
    for (int i = 0; i < 10; i++)
    {
        insert(keys_arr[i], values_arr[i]);
    }

    search(root, 19);
}

static void unit_test_internal_split()
{
    for (int i = 10; i < 14; i++)
    {
        insert(keys_arr[i], values_arr[i]);
    }
    
    // search(root, 19);
    // search(root, 29);
    search(root, 43);
    // search(root, 13);
}

int main() 
{
    initialize_values_arr();

    // leaf split WORKS
    // unit test1 DONE
    // unit_test_leaf_split();
    printf("-----------------A new test case-----------------\n");
    // root split WORKS
    // unit test2 DONE
    unit_test_root_split();
    printf("-----------------A new test case-----------------\n");
    // internal split WORKS
    // unit test3 DONE
    unit_test_internal_split();
    printf("-----------------A new test case-----------------\n");

    
    return 0;
}