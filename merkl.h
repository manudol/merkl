#ifndef MERKL_H
#define MERKL_H


#define MBPL 64 
#define FILENAME "seq0.txt"

int count_chars();

typedef struct node {
    char* id;
    char* seq;
    char* hash;
    bool is_leaf;
    struct node* left;
    struct node* right;
} node_t;

char* sha256(unsigned char const* dna_chunk);
node_t* new_leaf(char* id, char* dna_chunk);
node_t* init_tree(node_t* new_leaf);
void free_tree(node_t* node);
bool right_add_to_tree(node_t* head, node_t* new_leaf);

void get_bases(int n_leaves, int size_leaf, char leaves_arr[n_leaves][size_leaf]);
unsigned int get_n_layers(const unsigned int n_leaves);

#endif


char* sha256(unsigned char const* dna_chunk)
{
    if (dna_chunk == NULL) {
        perror("Please provide a string to hash.\n");
        exit(1);
    }
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    char* hash = (char*) malloc(EVP_MAX_MD_SIZE * 2 + 1);
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
    EVP_DigestUpdate(mdctx, dna_chunk, strlen((const char*) dna_chunk));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_destroy(mdctx);

    for(i = 0; i < md_len; i++)
        sprintf(hash + (i * 2), "%02x", md_value[i]);

    hash[md_len * 2] = '\0';
    EVP_cleanup();
    return hash;
}


node_t* new_leaf(char*id, char* dna_chunk)
{
    char* hash = sha256((unsigned char const*)dna_chunk);
    node_t* leaf  = (node_t*) malloc(sizeof(node_t));
    leaf->id      = id;
    leaf->seq     = dna_chunk;
    leaf->hash    = hash;
    leaf->is_leaf = true;
    leaf->left    = NULL;
    leaf->right   = NULL;
    return leaf;
}

node_t* init_tree(node_t* new_leaf)
{
    char* hash = sha256((unsigned char const*)new_leaf->hash);
    node_t* head = (node_t*) malloc(sizeof(node_t));
    head->id      = NULL;
    head->seq     = NULL;
    head->hash    = hash;
    head->is_leaf = false;
    head->left    = new_leaf;
    head->right   = NULL;
    return head;
}


bool right_add_to_tree(node_t* head, node_t* new_leaf)
{
    node_t* curr = head;
    if (curr->right == NULL && !curr->is_leaf) {
        curr->right = new_leaf;
        return true;
    } else if (curr->right != NULL && !curr->is_leaf) {
        right_add_to_tree(curr->right, new_leaf);
    } else if (curr->right == NULL && curr->is_leaf) {
        return false;
    }
    return false;
}


void update_hash(node_t* head)
{
    
}


// node_t* add_leaf(char* id, char* dna_chunk)
// {
//     node_t* leaf = new_leaf(id, dna_chunk);
// }


void free_tree(node_t* node)
{
    if (node->right != NULL) {
        free_tree(node->right);
    }
    if (node->left != NULL) {
        free_tree(node->left);
    }
    if (node->hash != NULL) {
        free(node->hash);
    }
    free(node);
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


