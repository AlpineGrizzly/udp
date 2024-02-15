/**
 * rsa_dec.h
 * 
 * Header for RSA decryption function declarations 
 * 
 * Author Dalton Kinney
 * Created Feb 4th, 2024
*/
#include <stdio.h>
#include <stdlib.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "rsa_dec.h"

#define MAX_BUF 2048
#define DEBUG 1

static RSA* read_privkey(char* filename) { 
    // Open the private key pem file to be read
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Error opening private key file");
        return NULL;
    }

    // Read the base64 decoded PEM file into the RSA structure
    RSA* rsa_private_key = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);

    if (!rsa_private_key) {
        perror("Unable to get private key from pem file");
        fclose(f);
        return NULL;
    }
    fclose(f);

#ifdef DEBUG
    // Print the Private key mod and expo
    const BIGNUM *n, *e;
    RSA_get0_key(rsa_private_key, &n, &e, NULL);

    char *n_str = BN_bn2hex(n);
    char *e_str = BN_bn2hex(e);

    printf("Private key modulus (n):\n%s\n", n_str);
    printf("Private key exponent (e):\n%s\n", e_str);

    OPENSSL_free(n_str);
    OPENSSL_free(e_str);
#endif

    return rsa_private_key;
}

int rsa_dec(char* data, int data_len, char* priv_key, char* outfile) { 
    unsigned char dec_data[MAX_BUF] = {0}; // Initialize buffer to hold out decrypted data

#ifdef DEBUG
    printf("Reading key from %s and decrypting: %s\n", priv_key, data);
#endif

    // Read the private key from the PEM file 
    RSA *priv = read_privkey(priv_key);
    if (!priv) { 
        printf("Unable to decrypt\n");
        return 0;
    }

    // Decrypt data with the private key
    int dec_len = RSA_private_decrypt(data_len, (unsigned char*)data, dec_data, priv, RSA_PKCS1_PADDING);
    if (dec_len == -1) { 
        printf("Error decrypting data\n");
        RSA_free(priv);
        return 0;
    }

    #ifdef DEBUG
    // Print decryted data  
    printf("Decrypted data: %s\n", (char*)dec_data);
    #endif

    // Write to output file 
    FILE* out = fopen(outfile, "wb");
    if (!out) {  
        printf("Unable to write out to %s\n", outfile);
        RSA_free(priv);
        return 0;
    }
    
    size_t bw = fwrite(dec_data, sizeof(unsigned char), dec_len, out);
    if (bw != dec_len) { 
        printf("Error occurred while writting file\n");
    }
    fclose(out);

    RSA_free(priv);
    return data;
}