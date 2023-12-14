#define _POSIX_C_SOURCE 200809
#include <time.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/wait.h>

#include "libfun.h"


void print_message(char * message) {
    write(STDOUT_FILENO, message, strlen(message));
}

void print_error(char * message) {
    write(STDERR_FILENO, message, strlen(message));
}

char *str_ip(void *addr) { // Get ip as string
    char *ip_str = (char *)malloc(INET6_ADDRSTRLEN);
    if (ip_str == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    inet_ntop(AF_INET, addr, ip_str, INET6_ADDRSTRLEN);
    return ip_str;
}


char * get_ip(char * host) { // Get ip from host url
    struct addrinfo hints, *serverinfo, *p;
    int status;

    // 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // Datagrams

    // 
    if ((status = getaddrinfo(host, PORT, &hints, &serverinfo)) != 0) {
        print_error(GET_ADDR_INFO_ERROR);
        exit(EXIT_FAILURE);
    }
 
    // 
    for (p = serverinfo; p != NULL; p = p->ai_next) {
        void *addr;
        char ipstr[INET6_ADDRSTRLEN];

        
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }

        
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr); // Get ip as str

        char message[500]; // tmp print
        sprintf(message, "IP Address of TFTP : %s\n", ipstr);
        print_message(message);

        freeaddrinfo(serverinfo);
        return addr;
    }

    freeaddrinfo(serverinfo);
    return NULL;
}

void get_socket(char * ip) { // Get socket from ip
    int sockfd;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(PORT));

    // Convert IP address string to binary form
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        print_error(IP_INVALID_ERROR);
        exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        print_error(SOCKET_CREATE_ERROR);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(sockfd);
        print_error(SOCKET_CONNECT_ERROR);
        exit(EXIT_FAILURE);
    }

    print_message("Connected\n");


    close(sockfd);
}


void gettftp(char * host, char * file) {

    file = file; // tmp 
    void * addr = get_ip(host);
    char * str_addr = str_ip(addr);
    get_socket(str_addr);
    struct tftp_rrq rrq_packet;
    rrq_packet.opcode = htons(RRQ_OPCODE);
    strcpy(rrq_packet.filename_and_mode, file);
    sendto(sockfd, &rrq_packet, sizeof(uint16_t) + strlen(file) + 1, 0,
           (struct sockaddr *)&str_addr, sizeof(str_addr));
    
}

void puttftp(char * host, char * file) {
    host = host; // tmp
    file = file; // tmp
    print_message("puttftp test \n");
}