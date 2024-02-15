/**
 * rsa_enc.c
 * 
 * implementation file for function definitions of RSA encryption 
 * 
 * Author Dalton Kinney
 * Created Feb 4th, 20234
*/
#include <stdio.h>
#include <stdlib.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "rsa_enc.h"

#define MAX_BUF 2048
#define DEBUG 1

static RSA* read_pubkey(char* filename) {     
    // Open the public key pem file to be read
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Error opening public key file");
        return NULL;
    }

    // Read the base64 decoded PEM file into the RSA structure
    RSA* rsa_public_key = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
    if (!rsa_public_key) {
        perror("Unable to get public key from pem file");
        fclose(f);
        return NULL;
    }
    fclose(f);

#ifdef DEBUG
    // Print the Public key mod and expo
    const BIGNUM *n, *e;
    RSA_get0_key(rsa_public_key, &n, &e, NULL);

    char *n_str = BN_bn2hex(n);
    char *e_str = BN_bn2hex(e);

    printf("Public key modulus (n):\n%s\n", n_str);
    printf("Public key exponent (e):\n%s\n", e_str);

    OPENSSL_free(n_str);
    OPENSSL_free(e_str);
#endif

    return rsa_public_key;
}

int rsa_enc(unsigned char* data, int data_len, char* pub_key, char* outfile) {
    unsigned char enc_data[MAX_BUF] = {0}; 

#ifdef DEBUG
    printf("Reading key from %s and encrypting: %s\n", pub_key, data);
#endif
    
    // Read the public key from the PEM file
    RSA *pub = read_pubkey(pub_key);
    if (!pub) { 
        printf("Unable to encrypt %s\n", data);
        return 0;
    }

    // Encrypt the data with the public key
    int enc_len = RSA_public_encrypt(data_len, (unsigned char*)data, enc_data, pub, RSA_PKCS1_PADDING);
    if (enc_len == -1) {
        printf("Error encrpyting data\n");
        RSA_free(pub);
        return 0;
    }

#ifdef DEBUG
    // Print encrypted text 
    printf("Encrypted message (in hexadecimal):\n");
    for (int i = 0; i < enc_len; ++i) {
        printf("%02x", enc_data[i]);
    }
    printf("\n");
#endif

    // Write to output file 
    FILE* out = fopen(outfile, "wb");
    if (!out) {  
        printf("Unable to write out to %s\n", outfile);
        RSA_free(pub);
        return 0;
    }
    
    size_t bw = fwrite(enc_data, sizeof(unsigned char), enc_len, out);
    if (bw != enc_len) { 
        printf("Error occurred while writting file\n");
    }
    fclose(out);

    // Free RSA structure
    RSA_free(pub);
    return 1; // Return our encrypted data
}