#include <stdint.h> 

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

int send_udp();