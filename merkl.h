#ifndef MERKL_H
#define MERKL_H

#define FILENAME "seq0.txt"

int count_chars();

typedef struct node {
    char* id;
    char* seq;
    bool is_leaf;
    int rnn; // Right Null Nodes 
    int lrnn_lvl; // lowest rnn level connected
    bool rnl; // Right Null Leaves
    int level; // leaf level == 0
    char* hash;
    struct node* left;
    struct node* right;
} node_t;

char* sha256(unsigned char const* dna_chunk);

node_t* new_leaf(char* id, char* dna_chunk);

node_t* init_tree(node_t* new_leaf);
void free_tree(node_t* node);

bool has_rnn(node_t* head);
bool has_rnl(node_t* head);

void find_rnl(node_t* head, int* path, int index, node_t** node_rnl);
void find_rnn(node_t* head, int* path, int* index, node_t** node_rnn);

void rnl_rehash(node_t* head, node_t* node_rnl, int* path);
node_t* add_leaf(node_t* head, node_t* new_leaf);

int num_layers(node_t* head);
void get_bases(int n_leaves, int size_leaf, char leaves_arr[n_leaves][size_leaf]);

#endif


