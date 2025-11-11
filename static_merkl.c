#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include "hash.h"
#include "merkl.h"
#include "static_merkl.h"

bool is_base2(int n)
{
    return (n & (n - 1)) == 0;
}


int get_nleaves(int n_elements)
{
    int n = n_elements;
    while(!is_base2(n)) {
        n++; 
    }
    return n;
}



node_t* create_leaf(char* data)
{
    node_t* node = malloc(sizeof(node_t));
    char* hash = sha256((const unsigned char*) data);
    
    node->data = data;
    node->is_leaf = true;
    node->rnn = 0;
    node->lrnn_lvl = 0;
    node->rnl = false;
    node->level = 0;
    node->hash = hash;
    return node;
}

void init_leaves_ptr_arr(int n_leaves,  char* str[], node_t* arr[])
{
    for (int i = 0; i < n_leaves; i++) {
        node_t* node = create_leaf(str[i]);
        arr[i] = node;
    }
}

node_t* init_static_tree(int n_leaves, node_t* arr[])
{
    int n_nodes = n_leaves >> 1;
    node_t* prev[n_leaves];
    for (int i = 0; i < n_leaves; i++) {
        prev[i] = arr[i];
    }
    node_t* head = NULL;

    while (n_nodes > 0) {
        for (int i = 0; i < n_nodes; i++) {
            char* hash1  = prev[2 * i]->hash;
            char* hash2  = prev[2 * i + 1]->hash;
            node_t* node = malloc(sizeof(node_t));
            node->data    = NULL;
            node->level   = log2(n_leaves) - log2(n_nodes);
            node->is_leaf = false;
            node->rnn     = 0;
            node->lrnn_lvl= 0;
            node->rnl     = false;
            node->hash    = sha256((const unsigned char*) concat_hash(hash1, hash2));
            node->left    = prev[2 * i];
            node->right   = prev[2 * i + 1];
            head = node;
            prev[i] = node;
        }

        n_nodes >>= 1;
    }
    return head;
}

// int main()
// {   
//     char* str[] = {"1", "2", "3", "4"};
//     int n_leaves = get_nleaves(sizeof(str) / sizeof(str[0]));
//     node_t* arr[n_leaves];
//     init_leaves_ptr_arr(n_leaves, str, arr);
//     node_t* head = init_static_tree(n_leaves, arr);
// 
//     free_tree(head);
//     
//     return 0;
// }
