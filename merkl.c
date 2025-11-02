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

    char* id2 = "id2874234377";
    char* chunk2 =  "UUUAAAACACACCCCCCAGCTAGCATCAGTCAGGATCGATC";

    char* id3 = "id2874234377";
    char* chunk3 =  "UAGCATGATGCTATGCCCCCCAGCTAGCATCAGTCAGGATCGATC";

    char* id4 = "id2874234377";
    char* chunk4 =  "UUUAAAACACACCCUUUUUUUCTAGCATCAGTCAGGATCGATC";

    char* id5 = "id2874234377";
    char* chunk5 =  "UUUAAAACACACCCUUUUUUUCTAGCATCAGTCAGGATCGATC";
    printf("\n======= tree 1 =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left hash = %s\n", head->left->hash);
    printf("\nRNL = %d\n", head->rnl);
    printf("\nRNN = %d\n", head->rnn);

    node_t* leaf1 = new_leaf(id1, chunk1);
    head = add_leaf(head, leaf1);

    printf("\n======= tree 2 =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left hash = %s\n", head->left->hash);
    printf("right hash = %s\n", head->right->hash);
    printf("\nRNL = %d\n", head->rnl);
    printf("\nRNN = %d\n", head->rnn);

    node_t* leaf2 = new_leaf(id2, chunk2);
    head = add_leaf(head, leaf2);
  
    printf("\n======= tree 3 =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left head hash = %s\n", head->left->hash);
    printf("right head hash = %s\n", head->right->hash);
    printf("left head left leaf hash = %s\n", head->left->left->hash);
    printf("left head right leaf hash = %s\n", head->left->right->hash);
    // printf("right head right leaf hash = %s\n", head->right->right->hash);
    printf("right head left leaf hash = %s\n", head->right->left->hash);
    printf("\nRNL = %d\n", head->rnl);
    printf("\nRNN = %d\n", head->rnn);

    node_t* leaf3 = new_leaf(id3, chunk3);
    head = add_leaf(head, leaf3);
  
    printf("\n======= tree 4  =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left head hash = %s\n", head->left->hash);
    printf("right head hash = %s\n", head->right->hash);
    printf("left head left leaf hash = %s\n", head->left->left->hash);
    printf("left head right leaf hash = %s\n", head->left->right->hash);
    printf("right head right leaf hash = %s\n", head->right->right->hash);
    printf("right head left leaf hash = %s\n", head->right->left->hash);
    printf("\nRNL = %d\n", head->rnl);
    printf("\nRNN = %d\n", head->rnn);

    node_t* leaf4 = new_leaf(id4, chunk4);
    head = add_leaf(head, leaf4);
  
    printf("\n======= tree 5  =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left head hash = %s\n", head->left->hash);
    printf("right head hash = %s\n", head->right->hash);
    printf("left head left hash = %s\n", head->left->left->hash);
    printf("left head left right leaf hash = %s\n", head->left->left->right->hash);
    printf("left head left left leaf hash = %s\n", head->left->left->left->hash);
    printf("left head right hash = %s\n", head->left->right->hash);
    // printf("right head right hash = %s\n", head->right->right->hash);
    printf("right head left hash = %s\n", head->right->left->hash);
    printf("right head left left leaf hash = %s\n", head->right->left->left->hash);
    printf("\nRNL = %d\n", head->rnl);
    printf("\nRNN = %d\n", head->rnn);

    node_t* leaf5 = new_leaf(id5, chunk5);
    head = add_leaf(head, leaf5);
  
    printf("\n======= tree 6  =======\n");
    printf("head hash = %s\n", head->hash);
    printf("left head hash = %s\n", head->left->hash);
    printf("right head hash = %s\n", head->right->hash);
    printf("left head left hash = %s\n", head->left->left->hash);
    printf("left head left right leaf hash = %s\n", head->left->left->right->hash);
    printf("left head left left leaf hash = %s\n", head->left->left->left->hash);
    printf("left head right hash = %s\n", head->left->right->hash);
    // printf("right head right hash = %s\n", head->right->right->hash);
    printf("right head left hash = %s\n", head->right->left->hash);
    printf("right head left left leaf hash = %s\n", head->right->left->left->hash);
    printf("right head left left leaf hash = %s\n", head->right->left->right->hash);
    printf("\nRNL = %d\n", head->rnl);
    printf("\nRNN = %d\n", head->rnn);
    free_tree(head);
    return 0;
}
