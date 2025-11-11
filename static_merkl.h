#ifndef STATIC_MERKL_H
#define STATIC_MERKL_H

int get_nleaves(int n_elements);
node_t* create_leaf(char* data);
void init_leaves_ptr_arr(int n_leaves,  char* str[], node_t* arr[]);
node_t* init_static_tree(int n_leaves, node_t* arr[]);

#endif
