# b-plus-tree-c
A B+tree Implementation in C

## Notes
* The only hardcoded parameter is p that is the order of the tree (i.e. maximum number of pointers on a node)
  * NOTE: node can have up to p+1 pointers to accomodate the overflow which will then be handled by node splits
  * The actual number of pointers := q where q \in [2, p] or ([2, p+1] illegally) for k >= 1 (k := number of actual keys)
  * k will always be eaqul to q - 1
* Level starts from 1 (i.e. root sits at level 1)
* For all search field values X in the subtree pointed at by P_i, we have K_{i−1} < X ≤ K_i for 1 < i < q; X ≤ K_i for i = 1; and K_{i−1} < X for i = q 
* Keys == search field values; values/data == data file record

## Quick Start
all you have to do is:
```
for (int i = 0; i < total_number_of_keys_you_want_to_insert; i++)
{
    insert(keys_arr[i], values_arr[i]);
}
```
in main.c

## TODOs
* single linked-list on the leaf level (if there is a need to implement it, which I don't find one yet)

## References
* Thanks to Boming Kong @bigjr-mkkong: https://oi-wiki.org/ds/bplus-tree/
* https://asolanki.co.in/wp-content/uploads/2019/02/Fundamentals_of_Database_Systems_6th_Edition-1.pdf
