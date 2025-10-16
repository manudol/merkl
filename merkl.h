#ifndef MERKL_H
#define MERKL_H


#define MBPL 64 
#define FILENAME "seq0.txt"

char *sha256(char str[64]);
// int count_chars();
// bool is_square(unsigned int n);

typedef struct node {
    char* hash;
    struct node* left;
    struct node* right;
} node_t;

typedef struct TreeInfo {
    int n_leaves;
    int leaf_size;
} tree_t;


tree_t *n_leaves_and_size();
void get_bases(int n_leaves, int size_leaf, char leaves_arr[n_leaves][size_leaf]);
unsigned int get_n_layers(const unsigned int n_leaves);
void create_tree(const unsigned int n_leaves, const unsigned int size_leaf, char leaves_arr[n_leaves][size_leaf], node_t *last_layer[n_leaves]);
void print_merkle(node_t *root[], const unsigned int n_leaves);
void print_leaves(int size_leaf, int n_leaves, char leaves_arr[n_leaves][size_leaf]);


#endif



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
    
    fclose(pt);
    return characters;
}


bool is_square(unsigned int n)
{
    return (n & (n - 1)) == 0; 
}




tree_t *n_leaves_and_size()
{
    const unsigned int n_chars = count_chars();
    tree_t *info = (tree_t*) malloc(sizeof(tree_t));
    unsigned int n_leaves = 0;
    unsigned int i = 0;
    for (i = MBPL; i > 0; i--) {
        if (n_chars % i == 0) {
            n_leaves = n_chars / i;
        } else {
            n_leaves = n_chars / i + 1; // +1 ceil division
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
    int char_counter = 0;
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        perror("file ptr is NULL"); 
        exit(1);
    }

    for (int i = 0; i < n_leaves; i++) {
        memset(leaves_arr[i], 0, size_leaf);
    }

    while((ch = fgetc(fp)) != EOF) {
        if (ch != '\n' && char_counter < n_leaves * size_leaf) {
            leaves_arr[char_counter / size_leaf][char_counter % size_leaf] = ch;
            char_counter++;
        }
    }
    
    fclose(fp);
}


unsigned int get_n_layers(const unsigned int n_leaves)
{
    return (unsigned int) (log2(n_leaves) + 1); // + 1 for the root node
}


 
void create_tree(const unsigned int n_leaves, const unsigned int size_leaf, char leaves_arr[n_leaves][size_leaf], node_t *last_layer[n_leaves])
{
    const unsigned int n_layers = get_n_layers(n_leaves);
    
    int n_groups = n_leaves;

    for (int i = 0; i < n_layers; i++) { 
        node_t *curr_layer[n_groups];
        for (int j = 0; j < n_groups; j++) {
            curr_layer[j] = (node_t*) malloc(sizeof(node_t));
            node_t *leaf_ptr = curr_layer[j];
            
            if (i == 0) {
                leaf_ptr->left  = NULL;
                leaf_ptr->right = NULL;
                
                if (j < n_leaves) {
                    char leaf_data[size_leaf + 1];
                    memcpy(leaf_data, leaves_arr[j], size_leaf);
                    leaf_data[size_leaf] = '\0';
                    
                    leaf_ptr->hash  = sha256(leaf_data);
                    char* hashprint = leaf_ptr->hash;
                } else {
                    leaf_ptr->hash = NULL;
                    printf("Layer %d: Invalid leaf index %d\n", i, j);
                }
            } else {
                if (j*2+1 < n_groups*2) {
                    leaf_ptr->left  = last_layer[j*2];
                    leaf_ptr->right = last_layer[j*2 + 1];
                    
                    if (leaf_ptr->left && leaf_ptr->right && 
                        leaf_ptr->left->hash && leaf_ptr->right->hash) {
                        
                        char new_hash[129]; 

                        strncpy(new_hash, leaf_ptr->left->hash, 64);
                        new_hash[64] = '\0'; 
                        strncat(new_hash, leaf_ptr->right->hash, 64);

                        leaf_ptr->hash = sha256(new_hash); 
                        char *hashprint = leaf_ptr->hash;
                    } else {
                        printf("Layer %d: Invalid child nodes for index %d\n", i, j);
                        leaf_ptr->hash = NULL;
                    }
                } else {
                    printf("Layer %d: Invalid child index for %d\n", i, j);
                    leaf_ptr->hash = NULL;
                }
            }
        }
        
        for (int j = 0; j < n_groups && j < n_leaves; j++) {
            last_layer[j] = curr_layer[j];
        }
        
        n_groups >>= 1;  
    }
}



void print_merkle(node_t *root[], const unsigned int n_leaves)
{
    const unsigned int n_layers = get_n_layers(n_leaves);
    const int hash_display_len = 5;
    
    const int n_chpernode = 8;
    const char separator[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    int base = 2;
    int curr_layer = 0;
    node_t *prev_node_layer[ n_leaves ];
    prev_node_layer[0] = *root;
    node_t *curr_node_layer[ n_leaves ];
    
    char *root_hash = root[0]->hash;
    root_hash[5] = '\0';
    for (int i = 0; i < n_leaves * (hash_display_len + strlen(separator)) / 2; i++) {
        printf(" ");
    }
    printf("%s", root_hash);
    for (int i = 0; i < n_leaves * (hash_display_len + strlen(separator)) / 2; i++) {
        printf(" ");
    }
    printf("\n");

    while (curr_layer != n_layers - 1) {
        int n_nodes = pow(base, curr_layer); // num nodes on the curr layer
        int n_spaces = n_leaves - n_nodes;
        
        for (int i = 0; i < n_spaces * (hash_display_len + strlen(separator)) / 2; i++) {
                printf(" ");
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
        
        for (int i = 0; i < n_spaces * (hash_display_len + strlen(separator)) / 2; i++) {
                printf(" ");
        }
        printf("\n");
        
        for (int i = 0; i < n_leaves; i++) {
            prev_node_layer[i] = curr_node_layer[i];
        }
        curr_layer++;
    }
}


void print_leaves(int size_leaf, int n_leaves, char leaves_arr[n_leaves][size_leaf])
{
    for (int i = 0; i < n_leaves; i++) {
        printf("Leaf %d: ", i+1);
        for (int j = 0; j < size_leaf; j++) {
            printf("%c", leaves_arr[i][j]);
        }
        printf("\n");
    }
}
