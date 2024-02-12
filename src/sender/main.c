#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "sender.h"


#define NUM_ARGS 7
#define DEBUG 1

#define BUFSIZE 2048

/** Argv enum for arguments */
enum Args{None, In_file, Src_ip, Dst_ip, Src_port, Dst_port, Out_file};

/**
 * usage
 * 
 * Prints the usage of the program
*/
void usage() { 
	char* usage_string = "Usage: sender [data filename] [src_ip] [dst_ip] [src_port] [dst_port] [datagram file name]\n" 
						 "UDP datagram. \n\n" 
                         "-h      Show this information\n";
    
	printf("%s", usage_string);
    exit(0);
}

int main(int argc, char* argv[]) { 
    char *datafile;
    unsigned long src_ip;
    unsigned long dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    char *output; 
    uint16_t checksum = 0; // To be calculated later

    /** Get command line arguments */
    if (argc != NUM_ARGS) usage();

    /* Get in and output filenames */
    datafile = argv[In_file];
    output = argv[Out_file];

    /* Get src and dst ips and port numbers */
    src_ip = inet_addr(argv[Src_ip]);
    src_port = atoi(argv[Src_port]);

    dst_ip = inet_addr(argv[Dst_ip]);
    dst_port = atoi(argv[Dst_port]);

    if (src_ip == INADDR_NONE || dst_ip == INADDR_NONE) { 
        printf("One or more invalid ip addresses: %s, %s\n", argv[Src_ip], argv[Dst_ip]);
        return 1;
    }

    printf("Source port: %d\nnDestination port: %d\n", src_port, dst_port);
    printf("Source IP: %ld\nDestination IP: %ld\n", src_ip, dst_ip);

    /** Read the data in binary format */
    FILE* f; 
    char buf[BUFSIZE];
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
        printf("%02X ", (unsigned char)buf[i]);
    }
    printf("\n");
#endif 

    /** Create and send UDP datagram */
    // Calculate total length
    int pad = len % 2;
    if (pad) {  // Pad data to 16 bit words
        for (int j = 0; j < pad; j++) { 
            buf[len+j] = 0x00;
        }
        buf[len+pad] = '\0';
    }
    printf("len %ld\n", len);
    printf("with padding %d bytes\n", pad);

    int gram_len = UDP_HEADER_SIZE + len; 
    int total_len = 12 + gram_len;
    printf("Total length(bytes) of datagram %d\n", gram_len);
    printf("Total length(bytes) of everything %d\n", total_len);

    // Create our udp struct with all values converted to network byte order
    udp_packet packet = {(int)src_ip, 
                         (int)dst_ip, 
                         0, 
                         17,
                         total_len, 
                         src_port, 
                         dst_port, 
                         gram_len, 
                         checksum};
    packet.data = malloc(len+pad+1); 
    strcpy(packet.data, buf);
   
    // Calculate check sum 
    uint32_t checkbuf = 0;
    // header
    for (int i = 0; i < sizeof(packet)-(len+pad); i+=2) { 
        checkbuf = (uint16_t)((unsigned char*)&packet)[i] << 8 | ((unsigned char*)&packet)[i+1];   
        printf("%04x\n", checkbuf);
    }

    // data
    for (int j = 0; j < len+pad; j+=2) { 
        checkbuf += buf[j] << 8 | buf[j+1];
        printf("%04x \n", checkbuf);
    }
    checksum = checksum >> 8 + (checksum | 0xffff);
    printf("%04x \n", checkbuf);
    // Convert to network byte order
    packet.src_port = htons(src_port);
    packet.dst_port = htons(dst_port);
    packet.len = htons(gram_len);
    packet.checksum = htons(checksum);
    
    FILE *fout = fopen("out.bin", "wb");
    printf("\n");
    printf("%08x ", packet.src_ip);
    printf("%08x ", packet.dst_ip);
    printf("%02x ", packet.zeros);
    printf("%02x ", packet.proto);
    printf("%04x \n\n", packet.total_len);

    printf("%04x ", packet.src_port);
    fwrite(&packet.src_port, sizeof(packet.src_port),1,fout);

    printf("%04x ", packet.dst_port);
    fwrite(&packet.dst_port, sizeof(packet.dst_port),1,fout);

    printf("%04x ", packet.len);
    fwrite(&packet.len, sizeof(packet.len),1,fout);

    printf("%04x ", packet.checksum);
    fwrite(&packet.checksum, sizeof(packet.checksum),1,fout);

    for (size_t i = 0; i < len+pad; i++) { 
        printf("%02x ", (unsigned char)packet.data[i]);
        fwrite(&packet.data[i], sizeof(unsigned char),1,fout);
    }
    printf("\n");

    //for (int i = 0; i < sizeof(packet); i++) { 
    //    printf("%02x ", ((unsigned char*)&packet)[i]);
    //}
    //printf("\n");
    //FILE *fout = fopen("out.bin", "wb");
    //fwrite(&packet, sizeof(packet),1,fout);
    fclose(fout);
    free(packet.data);
    // Calculate checksum over pseduo header and udp datagram and store in field
    // Order Src_ip(32), Dst_ip(32), zeros(8),Proto=17(8), Total length(16), Src_port(16), Dst_port(16),Total len(16),Check(16), Data
    
    

    int send = send_udp();
    return 0;
}