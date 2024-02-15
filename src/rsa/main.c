/**
 * main.c
 * 
 * RSA encryption/decryption tool using openssl libraries
 * 
 * Author Dalton Kinney
 * Created January 28th, 2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "rsa_enc.h"
#include "rsa_dec.h"

#define DEBUG 1
#define BUF_SIZE 2048

void replace_extension(char* filename, const char* new_extension) {
    char* dot = strrchr(filename, '.'); // Find the last occurrence of dot (.)
    if (dot != NULL) {
        *dot = '\0'; // Null-terminate the base name
    }
    strcat(filename, new_extension); // Append the new extension
}

/**
 * usage
 * 
 * Prints the usage of the program
*/
void usage() { 
	char* usage_string = "Usage: rsa [-d] -f filename -k public_key.pem | private_key.pem\n" 
						 "RSA encryption/decryption implementation. \n\n" 
                         "-h      Show this information\n"
                         "-f      File to be encrypted/decrypted\n"
                         "-k      Pem file containing public key | private key if `-d` flag is in use\n"
                         "-d      Flag to perform decryption on data file (If omitted, will assume file is to be encrypted\n";
	printf("%s", usage_string);
}

/**
 * get_data
 * 
 * Reads data from a file and stores it into a buffer
 * 
 * @param filename Name of file to read data from 
 * @param buf buffer for data to be read into 
 * 
 * @return 1 if successful, 0 otherwise
*/
int get_data(char* filename, unsigned char buf[BUF_SIZE]) { 
    FILE *pdata = fopen(filename, "r");

    if (pdata == NULL) { 
        return 0;
    }

    // Begin reading data into bu`ffer 
    int count = fread(buf, sizeof(unsigned char), BUF_SIZE, pdata);
    fclose(pdata);

    return count;
}

int main(int argc, char* argv[]) { 
    int opt;      // Used to hold args as we parse
    int mode = 0; // Holds mode for enc/dec (0 = Enc; 1 = Dec)  
    int success;  // Success of enc/dec 
    char *ext;    // Extension for output file depending on enc/dec mode
    char* datafile = NULL; // Filename of data to be enc/decr 
    char* keyfile  = NULL; // Filename of public/private pem file

	
    /* Parse arguments */ 
	while ((opt = getopt(argc, argv, "hf:k:d")) != -1) {
        switch (opt) {
        	case 'h': usage(); break;
        	case 'f': datafile = optarg; break;
            case 'k': keyfile  = optarg; break;
            case 'd': mode = 1; break; // Enable decryption mode
        default:
            usage();
        }
    }

    /** Verfiy arguments */
	if (datafile == NULL || keyfile == NULL) { 
        usage();
        return 0;
    }

    /** Read data from file */
    unsigned char data[BUF_SIZE] = {0}; // Data buffer
    int data_len = get_data(datafile, data); // Read in data from data file

    if (!data_len) {
        printf("Unable to read %s!\n", datafile);
        return 0;
    }
     
    /** Perform enc/dec and write to file if successful */
    if (mode) { 
        ext = ".bin"; // Replace extension 
        replace_extension(datafile, ext);
        success = rsa_dec(data, data_len,  keyfile, datafile);       
    } else { 
        ext = ".enc";
        replace_extension(datafile, ext);
        success = rsa_enc(data, data_len,  keyfile, datafile);

    }

    if (!success) { 
        printf("Enc/Dec failed!\n");
        return 0;
    } 

    printf("Output written to %s\n", datafile);
    return 0;
}
