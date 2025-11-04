#include <openssl/evp.h>
#include <string.h>

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
