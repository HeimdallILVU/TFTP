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
    hints.ai_family = AF_INET; // IPv4
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

int get_socket(char * ip) { // Get socket from ip
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

    return sockfd;

}

void print_raw_byte(char * packet, int packetSize) {
    // Print the rrqPacket as hexadecimal values
    for (int i = 0; i < packetSize; ++i) {
        char message[512];
        sprintf(message, "%02X ", packet[i]);
        print_message(message);
    }

    printf("\n");
}



void sendTFTPReadRequest(const char *filename, int sockfd) {
    // Prepare the RRQ packet
    char rrqPacket[516]; // Max size for a TFTP packet
    memset(rrqPacket, 0, sizeof(rrqPacket));

    // Opcode (Read Request)
    rrqPacket[0] = (TFTP_OPCODE_RRQ_READ >> 8) & 0xFF;
    rrqPacket[1] = TFTP_OPCODE_RRQ_READ & 0xFF;

    // Filename
    strcpy(&rrqPacket[2], filename);

    // Null terminator after the filename
    int filenameLength = strlen(filename);
    rrqPacket[2 + filenameLength] = '\0';

    // Mode
    strcpy(&rrqPacket[2 + filenameLength + 1], TFTP_MODE);

    // Null terminator after the mode
    int modeLength = strlen(TFTP_MODE);
    rrqPacket[2 + filenameLength + 1 + modeLength] = '\0';

    //print_raw_byte(rrqPacket, sizeof(rrqPacket));

    // Send the RRQ packet
    ssize_t bytesSent = send(sockfd, rrqPacket, sizeof(rrqPacket), 0);
    if (bytesSent == -1) {
        print_error(ERROR_SENDING_PACKET);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (DEBUG) print_message(MESSAGE_SUCCESS_READ_REQUEST);
}

void sendTFTPWriteRequest(const char *filename, int sockfd) {
    // Prepare the RRQ packet
    char rrqPacket[516]; // Max size for a TFTP packet
    memset(rrqPacket, 0, sizeof(rrqPacket));

    // Opcode (Write Request)
    rrqPacket[0] = (TFTP_OPCODE_RRQ_WRITE >> 8) & 0xFF;
    rrqPacket[1] = TFTP_OPCODE_RRQ_WRITE & 0xFF;

    // Filename
    strcpy(&rrqPacket[2], filename);

    // Null terminator after the filename
    int filenameLength = strlen(filename);
    rrqPacket[2 + filenameLength] = '\0';

    // Mode
    strcpy(&rrqPacket[2 + filenameLength + 1], TFTP_MODE);

    // Null terminator after the mode
    int modeLength = strlen(TFTP_MODE);
    rrqPacket[2 + filenameLength + 1 + modeLength] = '\0';

    //print_raw_byte(rrqPacket, sizeof(rrqPacket));

    // Send the RRQ packet
    ssize_t bytesSent = send(sockfd, rrqPacket, sizeof(rrqPacket), 0);
    if (bytesSent == -1) {
        print_error(ERROR_SENDING_PACKET);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (DEBUG) print_message(MESSAGE_SUCCESS_WRITE_REQUEST);
}

void sendTFTPData(char * data, int block_number, int sockfd) {
    // Prepare the RRQ packet
    char rrqPacket[516]; // Max size for a TFTP packet
    memset(rrqPacket, 0, sizeof(rrqPacket));

    // Opcode (Data)
    rrqPacket[0] = (TFTP_OPCODE_DATA >> 8) & 0xFF;
    rrqPacket[1] = TFTP_OPCODE_DATA & 0xFF;

    // Block Number
    rrqPacket[2] = (block_number >> 8) & 0xFF;
    rrqPacket[3] = block_number & 0xFF;

    // Data
    strcpy(&rrqPacket[4], data);

    // Null terminator after the filename
    int dataLength = strlen(data);
    rrqPacket[4 + dataLength] = '\0';

    //print_raw_byte(rrqPacket, sizeof(rrqPacket));

    // Send the RRQ packet
    ssize_t bytesSent = send(sockfd, rrqPacket, sizeof(rrqPacket), 0);
    if (bytesSent == -1) {
        print_error(ERROR_SENDING_PACKET);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (DEBUG) print_message(MESSAGE_SUCCESS_DATA);
}

void sendTFTPAck(int block_number, int sockfd) {
    // Prepare the RRQ packet
    char rrqPacket[516]; // Max size for a TFTP packet
    memset(rrqPacket, 0, sizeof(rrqPacket));

    // Opcode (ACK)
    rrqPacket[0] = (TFTP_OPCODE_ACK >> 8) & 0xFF;
    rrqPacket[1] = TFTP_OPCODE_ACK & 0xFF;

    // Block Number
    rrqPacket[2] = (block_number >> 8) & 0xFF;
    rrqPacket[3] = block_number & 0xFF;

    print_raw_byte(rrqPacket, sizeof(rrqPacket));

    // Send the RRQ packet
    ssize_t bytesSent = send(sockfd, rrqPacket, sizeof(rrqPacket), 0);
    if (bytesSent == -1) {
        print_error(ERROR_SENDING_PACKET);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (DEBUG) print_message(MESSAGE_SUCCESS_ACK);
}

void receivePacket(int sockfd, char * ip) {
    // Receive and save the file
    int blockNumber = 1;
    print_message("start waiting receive\n");
    while (1) {
        char dataPacket[TFTP_BLOCK_SIZE + 4]; // Opcode (2 bytes) + Block Number (2 bytes) + DATA

        // Server details
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(atoi(PORT)); // TFTP port
        inet_pton(AF_INET, ip, &(serverAddr.sin_addr));

        ssize_t bytesRead = recvfrom(sockfd, dataPacket, sizeof(dataPacket), 0, (struct sockaddr *) &serverAddr, (socklen_t *) sizeof(serverAddr));
        if (bytesRead == -1) {
            perror("Error receiving data packet");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        print_message("message");

        // Extract Opcode and Block Number
        int opcode = (dataPacket[0] << 8) | dataPacket[1];
        int receivedBlockNumber = (dataPacket[2] << 8) | dataPacket[3];

        if (opcode == TFTP_OPCODE_DATA && receivedBlockNumber == blockNumber) {
            // Write data to the file using write and file descriptor
            int bytesWritten = write(STDOUT_FILENO, &dataPacket[4], bytesRead - 4);

            if (bytesWritten == -1) {
                print_error(WRITE_ERROR);
                close(sockfd);
                exit(EXIT_FAILURE);
            }

            // Send ACK for the received block
            sendTFTPAck(blockNumber, sockfd);

            // Increment block number for the next iteration
            blockNumber++;

            // Check if the received data packet is less than the maximum size, indicating the last block
            if (bytesRead < TFTP_BLOCK_SIZE + 4) {
                break;
            }
        } else {
            // Ignore invalid packets (e.g., duplicates)
        }
    }
}


void gettftp(char * host, char * file) {
    file = file; // tmp 
    void * addr = get_ip(host);
    char * str_addr = str_ip(addr);
    int sockfd = get_socket(str_addr);

    if (DEBUG) print_message("Connected\n");
    sendTFTPReadRequest(file, sockfd);
    receivePacket(sockfd, str_addr);

    close(sockfd); 
      
}
 
void puttftp(char * host, char * file) {
    host = host; // tmp
    file = file; // tmp
    if (DEBUG) print_message("puttftp test \n");
} 