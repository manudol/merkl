#ifndef MERKL_H
#define MERKL_H

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
void right_update_hash(node_t* head);
// void left_hash_update(node_t* head, node_t* new_leaf);
int num_layers(node_t* head);
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


char* concat_hash(char* hash1, char* hash2)
{
    size_t len1     = strlen(hash1);
    size_t len2     = strlen(hash2);
    size_t totallen = len1 + len1 + 1;

    char* new_hash = (char*) malloc(totallen);
    strcpy(new_hash, hash1);
    strcpy(new_hash, hash2);
    return new_hash;
}

void r_right_update(node_t* head, node_t* node)
{
    char* hash1 = node->left->hash; // leaf hash left
    char* hash2 = node->right->hash; // leaf hash right
    
    char* new_hash = concat_hash(hash1, hash2);
    free(node->hash);
    node->hash = new_hash;

    if (node == head) {
        return;
    }

    node_t* curr = head;
    node_t* prev;
    while (curr != node) {
        prev = curr;
        curr = curr->right;
    }
    r_right_update(head, prev);
}


void right_update_hash(node_t* head)
{
    node_t* curr = head;
    node_t* prev;
    while (!curr->is_leaf) {
        prev = curr;
        curr = curr->right;
    }
    r_right_update(head, prev);
}



int num_layers(node_t* head)
{
    int layers = 0;
    node_t* curr = head;
    while (!curr->is_leaf) {
        curr = curr->left;
        layers++;
    }
    return layers;
}


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


