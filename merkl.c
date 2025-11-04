#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/evp.h>
#include <stdbool.h>

#include "merkl.h"

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
    unsigned int i;

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
    leaf->lrnn_lvl= 0;
    leaf->rnl     = false;
    leaf->hash    = hash;
    leaf->left    = NULL;
    leaf->right   = NULL;
    return leaf;
}


void update_branch(node_t* node, node_t* new_leaf, int* path, int lrnn_lvl, bool ignore_path)
{
    node_t* prev;
    node_t* curr = node;

    int i = 0;

    while (curr != new_leaf) {
        prev = curr;
        if (ignore_path) {
            curr = curr->left;
        } else {
            if (path[i] == 0) {
                curr = curr->left;
            } else if (path[i] == 1) {
                curr = curr->right;
            }
        }
        i++;
    }

    prev->level   = curr->level + 1;
    prev->is_leaf = false;
    prev->rnn     = lrnn_lvl > 0;
    prev->lrnn_lvl= lrnn_lvl;
    prev->rnl     = true;
    
    if (node != prev) {
        update_branch(node, prev, path, lrnn_lvl, ignore_path);
    }
}

node_t* init_tree(node_t* new_leaf)
{
    char* hash = sha256((unsigned char const*)new_leaf->hash);
    node_t* head = (node_t*) malloc(sizeof(node_t));
    head->id      = NULL;
    head->seq     = NULL;
    head->level   = 1;
    head->is_leaf = false;
    head->rnn     = 0;
    head->lrnn_lvl= 0;
    head->rnl     = true;
    head->hash    = hash;
    head->left    = new_leaf;
    head->right   = NULL;
    return head;
}

bool has_rnl(node_t* head)
{
    if (head->rnl > 0) {
        return true; 
    }
    return false;
}


bool has_rnn(node_t* head)
{
    if (head->rnn > 0) {
        return true; 
    }
    return false;
}

void find_rnl(node_t* head, int* path, int index, node_t** node_rnl)
{
    node_t* curr = head;
    if (!has_rnl(curr)) {
        perror("cannot call find_rnl since there are no Right Null leaves\n");
        exit(1);
    }

    if (curr->level == 1 && curr->right == NULL) {
        path[index] = 1;
        *node_rnl = curr;
    } else if (curr->left->rnl) {
        path[index] = 0;
        find_rnl(curr->left, path, index+1, node_rnl);
    } else if (curr->right->rnl) {
        path[index] = 1;
        find_rnl(curr->right, path, index+1, node_rnl);
    } 
}

void find_rnn(node_t* head, int* path, int* index, node_t** node_rnn)
{
    node_t* curr = head;
    if (!has_rnn(curr)) {
        perror("cannot call find_rnn since there are no Right Null leaves\n");
        exit(1);
    }

    if (curr->level == curr->lrnn_lvl && curr->right == NULL) {
        path[*index] = 1;
        *node_rnn = curr;
    } else if (curr->left->rnn > 0) {
        path[*index] = 0;
        int i = *index + 1;
        *index = i;
        find_rnn(curr->left, path, index, node_rnn);
    } else if (curr->right->rnn > 0) {
        path[*index] = 1;
        int i = *index + 1;
        *index = i;
        find_rnn(curr->right, path, index, node_rnn);
    }
}

char* concat_hash(char* hash1, char* hash2)
{
    size_t len1     = strlen(hash1);
    size_t len2     = strlen(hash2);
    size_t totallen = len1 + len2 + 1;

    char* new_hash = (char*) malloc(totallen);
    strcpy(new_hash, hash1);
    strcpy(new_hash, hash2);
    return new_hash;
}


void newhash(node_t* node)
{
    char* new_hash;
    if (node->right == NULL) {
        new_hash = sha256((const unsigned char*) node->left->hash);
    } else {
        new_hash = sha256((const unsigned char*) concat_hash(node->left->hash, node->right->hash));
    }
    free(node->hash);
    node->hash = new_hash;

}

void rnl_rehash(node_t* head, node_t* node_rnl, int* path)
{
    node_t* curr = head;
    node_t* stop = node_rnl;

    node_t* prev;

    int i = 0;

    stop->rnl = false;

    while (curr != stop) {
        if (path[i] == 0) {
            prev = curr;
            curr = curr->left;
        } else if (path[i] == 1) {
            prev = curr;
            curr = curr->right; 
        }
        i++;
    }

    newhash(curr);
    if (curr != head) {
        rnl_rehash(head, prev, path);
    }
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


int find_branch_rnn(int head_lvl)
{
    if (head_lvl == 1) {
       return 0;
    } else if (head_lvl > 1) {
        return 1;
    }
    return 0;
}

//void new_branch_rnn(node_t* rn_node, node_t* new_leaf, int depth, int rnn, int start, int* path, int* index)
void new_branch_rnn(node_t* rn_node, node_t* new_leaf, int depth, int start, int* path, int* index)
{
    node_t* prev = rn_node;

    if (depth > 1) {
        node_t* curr = (node_t*) malloc(sizeof(node_t));
        if (depth == start) {
            printf("depth == start\n");
            prev->right = curr;
            path[*index] = 1;
        } else if (depth < start) {
            printf("depth < start\n");
            prev->left = curr;
            path[*index] = 0;
        }
        int i = *index + 1;
        *index = i;
        new_branch_rnn(curr, new_leaf, depth - 1, start, path, index);
        //new_branch_rnn(curr, new_leaf, depth - 1, find_branch_rnn(depth - 1), start, path, index);
    }
    else if (depth == 1) {
        path[*index] = 0;

        int i = *index + 1;
        *index = i;

        prev->left = new_leaf; 
    }
}


// void new_branch(node_t* right_head, node_t* new_leaf, int depth, int rnn)
void new_branch(node_t* right_head, node_t* new_leaf, int depth)
{
    node_t* prev = right_head;

    if (depth > 1) {
        node_t* curr = (node_t*) malloc(sizeof(node_t));
        prev->left = curr;
        // new_branch(curr, new_leaf, depth - 1, find_branch_rnn(depth - 1));
        new_branch(curr, new_leaf, depth - 1);
    }
    else if (depth == 1) {
        prev->left = new_leaf; 
    }
}

void new_branch_rehash_rnn(node_t* head, node_t* new_leaf, int* path)
{
    node_t* prev;
    node_t* curr = head;

    int i = 0;

    while (curr != new_leaf) {
        prev = curr;
        if (path[i] == 0) {
            curr = curr->left;
        } else if (path[i] == 1) {
            curr = curr->right;
        }
        i++;
    }

    if (prev->right != NULL && prev->left != NULL) {
        prev->hash = sha256((const unsigned char *) concat_hash(prev->left->hash, prev->right->hash));
    } else if (prev->right == NULL && prev->left != NULL) {
        prev->hash = sha256((const unsigned char *)curr->hash);
    }

    if (prev != head) {
        new_branch_rehash_rnn(head, prev, path);
    }
}


void new_branch_rehash(node_t* right_head, node_t* new_leaf)
{
    node_t* prev;
    node_t* curr = right_head;
    while (curr != new_leaf) {
        prev = curr;
        curr = curr->left;
    }
    prev->hash = sha256((const unsigned char *)curr->hash);
    if (prev != right_head) {
        new_branch_rehash(right_head, prev);
    }
}


node_t* new_head(node_t* head, node_t* right_head)
{
    node_t* new_h = (node_t*) malloc(sizeof(node_t));
    new_h->left = head;
    new_h->right = right_head;

    new_h->level   = head->level + 1;
    new_h->is_leaf = false;
    new_h->rnn     = right_head->rnn;
    new_h->lrnn_lvl= right_head->lrnn_lvl;
    new_h->rnl     = right_head->rnl;
    
    char* head_hash = sha256((const unsigned char*) concat_hash(head->hash, right_head->hash));
    new_h->hash = head_hash;
    return new_h;
}

node_t* add_leaf(node_t* head, node_t* new_leaf) // returns the head
{
    node_t* h = head;
    int* path = (int *) malloc(head->level * sizeof(int));
    if (has_rnl(head)) {
        printf("has_rnl\n");
        node_t* node_rnl;
        find_rnl(head, path, 0, &node_rnl); 
        node_rnl->right  = new_leaf;
        rnl_rehash(head, node_rnl, path);
    } else if (has_rnn(head)) {
        printf("has_rnn\n");
        node_t* node_rnn;
        int index = 0;
        find_rnn(head, path, &index, &node_rnn); 
        int lrnn_lvl = node_rnn->level;
        if (lrnn_lvl == head->level - 1) {
            lrnn_lvl = 0;
        } else if (lrnn_lvl < head->level - 1) {
            lrnn_lvl = lrnn_lvl + 1;
        }
        // new_branch_rnn(node_rnn, new_leaf, node_rnn->level, find_branch_rnn(node_rnn->level), node_rnn->level, path, &index);
        new_branch_rnn(node_rnn, new_leaf, node_rnn->level, node_rnn->level, path, &index);
        new_branch_rehash_rnn(head, new_leaf, path);
        update_branch(head, new_leaf, path, lrnn_lvl, false);
    } else {
        printf("new_branch\n"); 
        node_t* right_head = (node_t*) malloc(sizeof(node_t));
        // new_branch(right_head, new_leaf, head->level, find_branch_rnn(head->level));
        new_branch(right_head, new_leaf, head->level);
        new_branch_rehash(right_head, new_leaf);
        int lrnn_lvl = 2;
        if (head->level == 1) {
            lrnn_lvl = 0;
        }
        update_branch(right_head, new_leaf, path, lrnn_lvl, true);
        h = new_head(head, right_head);
    }
    free(path);
    return h;
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


