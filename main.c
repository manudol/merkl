#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/evp.h>
#include <stdbool.h>

/** TODO:
* 1. find the right number of bases per leaves to both:
*   a) not go over the MBPL
*   b) get a base 2 leaf number
*   e.g.: for a MBPL = 10: 6 bases per leaf and 16 leaves.


* To do that:
* i)   get the num char first ✅
* ii)  make a small algo to respect both conditions ✅
* iii) generate the tree ✅
* iv)  print the tree
**/

#define MBPL 64 
#define FILENAME "seq0.txt"

typedef struct node {
    char* hash;
    struct node* left;
    struct node* right;
} node_t;


char *sha256(char str[64])
{
    if (str == NULL) {
        perror("Please provide a string to hash.\n");
        exit(1);
    }
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    char* hash = (char*) malloc(EVP_MAX_MD_SIZE);
    unsigned int md_len;
    int i;

    OpenSSL_add_all_digests();

    md = EVP_get_digestbyname("SHA256");
    if(!md) {
        printf("Unknown message digest \n");
        exit(1);
    }

    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str, strlen(str));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_destroy(mdctx);

    for(i = 0; i < md_len; i++)
        sprintf(hash + (i * 2), "%02x", md_value[i]);

    hash[md_len * 2] = '\0';
    EVP_cleanup();
    return hash;
}


int count_chars()
{
    FILE *pt = fopen(FILENAME, "r");

    if (pt == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char ch;
    int characters = 0;

    while ((ch = fgetc(pt)) != EOF) {
        if (ch != '\n')
            characters++;
    }

    return characters;
}


bool is_square(int n)
{
    return (n > 0) && ((n & (n - 1)) == 0); 
}


typedef struct TreeInfo {
    int n_leaves;
    int leaf_size;
} tree_t;


tree_t *n_leaves_and_size(const unsigned int n_chars)
{
    tree_t *info = (tree_t*) malloc(sizeof(tree_t));
    int n_leaves = 0;
    int i = 0;
    for (i = MBPL; i > 0; i--) {
        if (!(n_chars % i == 0)) {
            n_leaves = n_chars / i + 1;
        } else {
            n_leaves = n_chars / i;
        }

        if (is_square(n_leaves)) {
            info->n_leaves = n_leaves;
            info->leaf_size = i;
            return info;
        }
    
    }
    info->n_leaves = n_leaves;
    info->leaf_size = i;
    return info;
}

void get_bases(int n_leaves, int size_leaf, char leaves_arr[n_leaves][size_leaf]) 
{
    char ch;
    int counter = 0;
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {perror("file ptr is NULL"); exit(1);}

    while((ch = fgetc(fp)) != EOF) {
        if (ch != '\n')
            leaves_arr[counter / size_leaf][counter % size_leaf] = ch;
        
        counter++;
    }
}


void print_merkle(node_t *root[], int n_layers, int n_leaves)
{
    const int n_chpernode = 8;
    const char separator[3] = {' ', '|', ' '};
    int base = 2;
    int curr_layer = 0;
    node_t *prev_node_layer[ n_leaves ];
    prev_node_layer[0] = *root;
    node_t *curr_node_layer[ n_leaves ];
    
    char *root_hash = root[0]->hash;
    root_hash[5] = '\0';
    printf("                    %s                     \n", root_hash);
    
    while (curr_layer != n_layers - 1) {
        int n_nodes = pow(base, curr_layer); // num nodes on the curr layer
        int n_spaces = n_leaves - n_nodes;
        
        if (n_spaces > 2){
            for (int i = 0; i < n_spaces / 2 - 1; i++) {
                for (int j = 0; j < 8; j++) {
                    printf(" ");
                }
            }
        }
        
        for (int i = 0; i < n_nodes; i++) {
            node_t *ptr_left = prev_node_layer[i]->left;
            node_t *ptr_right = prev_node_layer[i]->right;
            curr_node_layer[i*2]   = ptr_left;
            curr_node_layer[i*2+1] = ptr_right;
            char *left_str  = ptr_left->hash; 
            char *right_str = ptr_right->hash;
            left_str[5] = '\0';
            right_str[5] = '\0';
            printf("%s%s%s", left_str, separator, right_str);
            if (i != n_nodes - 1) {
                printf("%s", separator);
            }
            free(prev_node_layer[i]->hash);
            free(prev_node_layer[i]);
        }
        
        if (n_spaces > 2){
            for (int i = 0; i < n_spaces / 2 - 1; i++) {
                for (int j = 0; j < 8; j++) {
                    printf(" ");
                }
            }
        }
        printf("\n");
        for (int i = 0; i < n_leaves; i++) {
            prev_node_layer[i] = curr_node_layer[i];
        }
        curr_layer++;
    }
}


int main()
{
    const unsigned int n_chars = count_chars();

    tree_t *tree_info = n_leaves_and_size(n_chars);
    const unsigned int n_leaves  = (const unsigned int) tree_info->n_leaves;
    const unsigned int size_leaf = (const unsigned int) tree_info->leaf_size;

    free(tree_info);

    printf("n_leaves: %i\n", n_leaves);
    printf("size_leaf: %i\n", size_leaf);

    char leaves_arr[n_leaves][size_leaf];
    get_bases(n_leaves, size_leaf, leaves_arr);

    const unsigned int n_layers = (const unsigned int) (log2(n_leaves) + 1); // + 1 for the root node
    printf("n_layers: %d\n", n_layers);

    int n_groups = n_leaves; 
    
    node_t *last_layer[n_leaves];
    last_layer[0] = NULL;

    for (int i = 0; i < n_layers; i++) { 
        node_t *curr_layer[n_groups];
        for (int j = 0; j < n_groups; j++) {
            curr_layer[j] = (node_t*) malloc(sizeof(node_t));
            node_t *leaf_ptr = curr_layer[j];
            
            if (last_layer[0] != NULL) {
                leaf_ptr->left  = last_layer[j*2];
                leaf_ptr->right = last_layer[j*2 + 1];
                
                char new_hash1[256];
                char new_hash2[128];
                
                const char *hash_ptr1  = leaf_ptr->left->hash;
                const char *hash_ptr2 = leaf_ptr->right->hash;
                
                strcpy(new_hash1, hash_ptr1);
                strcpy(new_hash2, hash_ptr2); 
                
                strcat(new_hash1, new_hash2);
                
                leaf_ptr->hash = sha256(new_hash1);
                char *leaf_hash = leaf_ptr->hash;
            } else {
                leaf_ptr->left  = NULL;
                leaf_ptr->right = NULL;
                leaf_ptr->hash  = sha256((char*)leaves_arr[j]);
                char* hashprint = leaf_ptr->hash;
            }
        }
        for (int j = 0; j < n_groups; j++) {
                last_layer[j] = curr_layer[j];
        }
        
        n_groups >>= 1;  
    }
    print_merkle(last_layer, n_layers, n_leaves);
    return 0;
}
