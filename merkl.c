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
    node_t* head = init_tree(leaf);

    char* id1 = "id2874234777";
    char* chunk1 =  "TACACACACACACCCCCCAGCTAGCATCAGTCAGGATCGATC";

    printf("\n======= before tree =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left hash = %s\n", head->left->hash);

    node_t* leaf1 = new_leaf(id1, chunk1);
    add_leaf(head, leaf1);

    printf("\n======= after tree =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left hash = %s\n", head->left->hash);
    printf("right hash = %s\n", head->right->hash);
  
    free_tree(head);
    return 0;
}
