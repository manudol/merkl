#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/evp.h>
#include <stdbool.h>

#include "merkl.h"


int main()
{
    char* id = "id2874234776";
    char* chunk =  "AGCGAGCTAGCATCAGTCAGGATCGATC";
    node_t* leaf = new_leaf(id, chunk);
    printf("left leaf = %p\n", leaf);
    printf("left leaf hash = %s\n", leaf->hash);
    node_t* head = init_tree(leaf);
    printf("head = %p\n", head);
    printf("head hash = %s\n", head->hash);

    char* id1 = "id2874234777";
    char* chunk1 =  "AGCGAGCTAGCATCAGTCAGGATCGATC";
    node_t* leaf1 = new_leaf(id1, chunk1);
    if (right_add_to_tree(head, leaf1)) {
        node_t* right = head->right;
        printf("right leaf= %p\n", right);
        printf("right leaf hash = %s\n", right->hash);

        right_update_hash(head);
    } 
 
    printf("head = %p\n", head);
    printf("head hash = %s\n", head->hash);

    int layers = num_layers(head);

    printf("num layers = %d\n", layers);
  
    free_tree(head);
    return 0;
}
