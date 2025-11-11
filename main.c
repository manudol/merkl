#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/evp.h>
#include <stdbool.h>

#include "merkl.h"
#include "static_merkl.h"
#include "data.h"

char* id = "id2874234776";
char* chunk =  "AGCGAGCTAGCATCAGTCAGGATCGATC";

char* id1 = "id2874234777";
char* chunk1 =  "TACACACACACACCCCCCAGCTAGCATCAGTCAGGATCGATC";

char* id2 = "id2874234377";
char* chunk2 =  "UUUAAAACACACCCCCCAGCTAGCATCAGTCAGGATCGATC";

char* id3 = "id2874234377";
char* chunk3 =  "UAGCATGATGCTATGCCCCCCAGCTAGCATCAGTCAGGATCGATC";

char* id4 = "id2874234377";
char* chunk4 =  "UUUAAAACACACCCUUUUUUUCTAGCATCAGTCAGGATCGATC";

char* id5 = "id2874234377";
char* chunk5 =  "UUUAAAACACACCCUUUUUUUCTAGCATCAGTCAGGAAAAAAAAA";

char* id6 = "id2874234377";
char* chunk6 =  "UUUAAAACCCUUUUUCCCCCCATCAGTCAGGAAAAAAAAA";

char* id7 = "id2874234377";
char* chunk7 =  "UUUAAAACCCUUUUUCCCCCCATCAGTCAGGAAAAAAAAA";

char* id8 = "id2874234377";
char* chunk8 =  "UUUAATTTATATATATAAACCCUUUUUCCCCCCATCAGTCAGGAAAAAAAAA";

node_t* leaf_static();
node_t* leaf_dynamic();

int main()
{
    node_t* h = leaf_static();
    node_t* h1 = leaf_dynamic();

    printf("static = %s\n", h->hash);
    printf("dynamic = %s\n", h1->hash);

    free_tree(h);
    free_tree(h1);
    return 0;
}

node_t* leaf_dynamic()
{
    node_t* leaf1 = new_leaf(id1, chunk1);
    node_t* head = init_tree(leaf1);

    node_t* leaf2 = new_leaf(id2, chunk2);
    head = add_leaf(head, leaf2);

    node_t* leaf3 = new_leaf(id3, chunk3);
    head = add_leaf(head, leaf3);

    node_t* leaf4 = new_leaf(id4, chunk4);
    head = add_leaf(head, leaf4);

    node_t* leaf5 = new_leaf(id5, chunk5);
    head = add_leaf(head, leaf5);

    node_t* leaf6 = new_leaf(id6, chunk6);
    head = add_leaf(head, leaf6);

    node_t* leaf7 = new_leaf(id7, chunk7);
    head = add_leaf(head, leaf7);

    node_t* leaf8 = new_leaf(id8, chunk8);
    head = add_leaf(head, leaf8);

    return head;    
}


node_t* leaf_static()
{
    char* id[] = {id1, id2, id3, id4, id5, id6, id7, id8};
    char* chunks[] = {chunk1, chunk2, chunk3, chunk4, chunk5, chunk6, chunk7, chunk8};
    int n_leaves = 8;
    char* serialized[n_leaves]; 
    for (int i = 0; i < n_leaves; i++) {
        serialized[i] = serialize_data(id[i], chunks[i]);
    }
    
    node_t* arr[n_leaves];
    init_leaves_ptr_arr(n_leaves, serialized, arr);
    node_t* head = init_static_tree(n_leaves, arr);

    return head;
}
