#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


#define NUM_ARGS 4
#define BUFSIZE 2048
#define DEBUG 1 
#define UDP_HEADER_SIZE 8 // in bytes

typedef struct { 
    // Pseudo Header
    unsigned int src_ip;
    unsigned int dst_ip;
    uint8_t zeros; // 0
    uint8_t proto; // 7
    uint16_t total_len;
    // Udp header
    uint16_t src_port; 
    uint16_t dst_port;
    uint16_t len;
    uint16_t checksum;
    // Data
    char* data;
} __attribute__((packed)) udp_packet;

/** Argv enum for arguments */
enum Args{None, Src_ip, Dst_ip, In_file};

/**
 * usage
 * 
 * Prints the usage of the program
*/
void usage() { 
	char* usage_string = "Usage: receiver [src_ip] [dst_ip] [datagram file name]\n" 
						 "UDP datagram receiver \n\n" 
                         "-h      Show this information\n";
    
	printf("%s", usage_string);
    exit(0);
}

int main(int argc, char* argv[]) { 
    char *datafile; 
    char *outfile = "out.txt";
    unsigned long src_ip;
    unsigned long dst_ip;

    /** Get command line arguments */
    if (argc != NUM_ARGS) usage();

    /* Get ingest file */
    datafile = argv[In_file];


    /* Get src and dst ips and port numbers */
    src_ip = inet_addr(argv[Src_ip]);
    dst_ip = inet_addr(argv[Dst_ip]);

    if (src_ip == INADDR_NONE || dst_ip == INADDR_NONE) { 
        printf("One or more invalid ip addresses: %s, %s\n", argv[Src_ip], argv[Dst_ip]);
        return 1;
    }
    
    /** Read the data in binary format */
    FILE* f; 
    unsigned char buf[BUFSIZE];
    size_t len; 

    f = fopen(datafile, "rb");
    if (f == NULL) { 
        perror("Unable to open datafile");
        return 1;
    }

    // Read binary data from file
    len = fread(buf, sizeof(char), sizeof(buf), f);
    if (ferror(f)) { 
        perror("Unable to read datafile");
        fclose(f);
        return 1;
    }
    fclose(f);

#ifdef DEBUG
    for (size_t i = 0; i < len; i++) { 
        printf("%02x ", buf[i]);
    }
    printf("\n");
#endif 

    // Generate pseudo header + udp payload
    uint16_t src_port  = (uint16_t)buf[0] << 8 | buf[1];
    uint16_t dst_port  = (uint16_t)buf[2] << 8 | buf[3];
    uint16_t gram_len  = (uint16_t)buf[4] << 8 | buf[5];
    uint16_t checksum  = (uint16_t)buf[6] << 8 | buf[7];

    // Create our udp struct with all values converted to network byte order
    udp_packet packet = {(int)src_ip, 
                         (int)dst_ip, 
                         0, 
                         17,
                         htons(gram_len), 
                         htons(src_port), 
                         htons(dst_port), 
                         htons(gram_len), 
                         checksum};

    // Do check sum 
    uint16_t calc_check = 0;
    uint32_t checkbuf = 0;
    uint16_t value = 0;

    // Pseudo header
    for (int i = 0; i < 12; i+=2) { 
        value = (uint16_t)((unsigned char*)&packet)[i] << 8 | ((unsigned char*)&packet)[i+1];
        checkbuf += value;
        //printf("%04x ", value);
    }
    //printf("\n");

    // Header and data
    // Over data
    for (int j = 0; j < len; j+=2) { 
        value = buf[j] << 8 | buf[j+1];
        checkbuf += value;
        //printf("%04x ", value);
    }
    //printf("\n");
    calc_check = ((uint16_t)(checkbuf >> 16) + checkbuf); 
    printf("Check sum is %04x\n", calc_check);

    if (calc_check != 0xffff) {
        printf("Checksum error!\n"); // Checksum error
        //exit(0);
    }
    // Datagram from source-address source-port to dest-address to dest-port Length xxx bytes
    printf("Datagram from %s:%d to %s:%d Length %d bytes\n", argv[Src_ip], src_port, argv[Dst_ip], dst_port, gram_len);

    // Write decrypted data to output file
    FILE *fout = fopen(outfile, "wb");
    for (int i = UDP_HEADER_SIZE; i < gram_len; i++) { 
        printf("%02x ", buf[i]);
        fwrite(&buf[i], sizeof(unsigned char),1,fout);
    }
    printf("\n");
    
    printf("Written to %s\n", outfile);
    fclose(fout);



}