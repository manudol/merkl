#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/evp.h>
#include <stdbool.h>

#include "merkl.h"



int main()
{
    tree_t *tree_info = n_leaves_and_size();
    const unsigned int n_leaves  = (const unsigned int) tree_info->n_leaves;
    const unsigned int size_leaf = (const unsigned int) tree_info->leaf_size;

    free(tree_info);

    printf("n_leaves: %i\n", n_leaves);
    printf("size_leaf: %i\n", size_leaf);

    char leaves_arr[n_leaves][size_leaf];
    get_bases(n_leaves, size_leaf, leaves_arr);

    const unsigned int n_layers = get_n_layers(n_leaves);
    printf("n_layers: %d\n", n_layers);
  
    node_t *last_layer[n_leaves];
    memset(last_layer, 0, sizeof(last_layer));
    
    create_tree(n_leaves, size_leaf, leaves_arr, last_layer);
    print_merkle(last_layer, n_leaves);
    //print_leaves(size_leaf, n_leaves, leaves_arr);
    return 0;
}
