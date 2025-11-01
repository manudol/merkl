#ifndef MERKL_H
#define MERKL_H

#define FILENAME "seq0.txt"

int count_chars();

typedef struct node {
    char* id;
    char* seq;
    bool is_leaf;
    int rnn; // Right Null Nodes 
    int rnl; // Right Null Leaves
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
bool has_rnl(node_t* head)

node_t* find_rnl(node_t* head)
node_t* find_rnn(node_t* head)

void right_update_hash(node_t* head);
// void left_hash_update(node_t* head, node_t* new_leaf);
int num_layers(node_t* head);
void get_bases(int n_leaves, int size_leaf, char leaves_arr[n_leaves][size_leaf]);

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
    leaf->level   = 0;
    leaf->is_leaf = true;
    leaf->rnn     = 0;
    leaf->rnl     = 0;
    leaf->hash    = hash;
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
    head->level   = 1;
    head->is_leaf = false;
    leaf->rnn     = 0;
    leaf->rnl     = 1;
    head->hash    = hash;
    head->left    = new_leaf;
    head->right   = NULL;
    return head;
}

bool has_rnl(node_t* head)
{
    node_t* curr = head;
    if (head->rnl > 0) {
        return true; 
    }
    return false;
}


bool has_rnn(node_t* head)
{
    node_t* curr = head;
    if (head->rnn > 0) {
        return true; 
    }
    return false;
}

node_t* find_rnl(node_t* head, int* path, int index)
{
    node_t* curr = head;
    if (!has_rnl(curr)) {
        perror("cannot call find_rnl since there are no Right Null leaves\n");
        exit(1);
    }

    if (curr->level == 1 && curr->right == NULL) {
        return curr;
    } else if (curr->left->rnl > 0) {
        path[index] = 0;
        find_rnl(curr->left, path, index+1);
    } else if (curr->right-rnl > 0) {
        path[index] = 1;
        find_rnl(curr->right, path, index+1);
    }
}

node_t* find_rnn(node_t* head)
{
    node_t* curr = head;
    if (!has_rnn(curr)) {
        perror("cannot call find_rnl since there are no Right Null leaves\n");
        exit(1);
    }

    if (curr->level > 1 && curr->right == NULL) {
        return curr;
    } else if (curr->left->rnn > 0) {
        find_rnn(curr->left);
    } else if (curr->right-rnn > 0) {
        find_rnn(curr->right);
    }
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

void add_leaf(node_t* head, node_t* new_leaf)
{
    int* path = (int *) malloc(head->level * sizeof(int));
    if (has_rnl(head)) {
        node_t* node_rnl = find_rnl(head, path, 0); 
        node_rnl->right  = new_leaf;
        // rehash(path);
    } else if (has_rnn(head)) {
        node_t* node_rnl = find_rnn(head, path, 0); 
        // rnl_add(head, node_rnl->level, path, 0);
    } else {
        // new_branch(new_leaf);
    }
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


node_t* nr_left_update(int n_layers, node_t* new_leaf)
{
    int i_layer  = n_layers;
    node_t* curr = new_leaf;
    while (i_layer > 0) {
        node_t* node = (node_t*) malloc(sizeof(node_t));
        node->id      = NULL;
        node->seq     = NULL;
        node->is_leaf = false;
        node->level   = n_layers + 1 - i_layer;
        node->hash    = sha256((const unsigned char*) curr->hash);
        node->left    = new_leaf;
        node->right   = NULL;

        curr = node;
        i_layer--;
    }
    return curr;
}


node_t* new_branch_update_hash(node_t* head, node_t* new_leaf)
{
    int n_layers = num_layers(head);
    
    node_t* right_head = nr_left_update(n_layers, new_leaf);
    node_t* left_head  = head;

    node_t* new_head  = (node_t*) malloc(sizeof(node_t));
    new_head->id      = NULL;
    new_head->seq     = NULL;
    new_head->is_leaf = false;
    new_head->level   = n_layers + 1;
    new_head->hash    = sha256((const unsigned char*) concat_hash(left_head->hash, right_head->hash));
    new_head->left    = left_head;
    new_head->right   = right_head;

    return new_head;
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


