#define _POSIX_C_SOURCE 200809
#include <time.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if (DEBUG) print_message(message);

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

void bind_socket_dyn(int sockfd) {
    // Get the bound port number
    struct sockaddr_in bound_addr;
    socklen_t addr_len = sizeof(bound_addr);
    if (getsockname(sockfd, (struct sockaddr*)&bound_addr, &addr_len) == -1) {
        perror("getsockname");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    bound_addr.sin_port = htons(atoi("0"));
        if (connect(sockfd, (struct sockaddr*)&bound_addr, sizeof(bound_addr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
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

    bind_socket_dyn(sockfd);


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
    bind_socket_dyn(sockfd);


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

void receivePacket(int sockfd, char * filename) {
    bind_socket_dyn(sockfd);

    // Receive and save the file
    int blockNumber = 1;

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(PORT)); 


    if (DEBUG) print_message("start waiting receive\n");

    int filefd = open(filename, O_CREAT | O_WRONLY);

    while (1) {
        char dataPacket[TFTP_BLOCK_SIZE + 4]; // Opcode (2 bytes) + Block Number (2 bytes) + DATA

        ssize_t bytesRead = recv(sockfd, dataPacket, sizeof(dataPacket), 0);
        if (bytesRead == -1) {
            print_error(RECEIVE_ERROR);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (DEBUG) print_message("message");

        // Extract Opcode and Block Number
        int opcode = (dataPacket[0] << 8) | dataPacket[1];
        int receivedBlockNumber = (dataPacket[2] << 8) | dataPacket[3];

        if (opcode == TFTP_OPCODE_DATA && receivedBlockNumber == blockNumber) {
            // Write data to the file using write and file descriptor
            int bytesWritten = write(filefd, &dataPacket[4], bytesRead - 4);

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

int receiveTFTPAckWRQ(int sockfd) {
    bind_socket_dyn(sockfd);

    char ackPacket[4]; // Opcode (2 bytes) + Block Number (2 bytes)
    ssize_t bytesRead = recv(sockfd, ackPacket, sizeof(ackPacket), 0);
    if (bytesRead == -1) {
        print_error("Error receiving ACK packet");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Extract Opcode and Block Number
    int opcode = (ackPacket[0] << 8) | ackPacket[1];
    int receivedBlockNumber = (ackPacket[2] << 8) | ackPacket[3];

    if (opcode == TFTP_OPCODE_ACK) {
        char message[MAX_OUTPUT_SIZE];
        sprintf(message, "ACK packet received for block number %d.\n", receivedBlockNumber);
        print_message(message);
        return receivedBlockNumber;
    } else {
        print_message("Error: Unexpected packet received.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

int receiveTFTPAckPacket(int sockfd) {
    bind_socket_dyn(sockfd);

    char ackPacket[4]; // Opcode (2 bytes) + Block Number (2 bytes)
    ssize_t bytesRead = recv(sockfd, ackPacket, sizeof(ackPacket), 0);
    if (bytesRead == -1) {
        print_error("Error receiving ACK packet");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Extract Opcode and Block Number
    int opcode = (ackPacket[0] << 8) | ackPacket[1];
    int receivedBlockNumber = (ackPacket[2] << 8) | ackPacket[3];

    if (opcode == TFTP_OPCODE_ACK) {
        char message[MAX_OUTPUT_SIZE];
        sprintf(message, "ACK packet received for block number %d.\n", receivedBlockNumber);
        print_message(message);
        return receivedBlockNumber;
    } else {
        print_message("Error: Unexpected packet received.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

void writePacket(int sockfd, char * filename) {
    // Get the bound port number
    struct sockaddr_in bound_addr;
    socklen_t addr_len = sizeof(bound_addr);
    if (getsockname(sockfd, (struct sockaddr*)&bound_addr, &addr_len) == -1) {
        perror("getsockname");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // Open the file for reading
    int filefd = open(filename, O_RDONLY);
    if (!filefd) {
        print_error("Error opening file for reading");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send file data in DATA packets
    int blockNumber = 1;
    while (1) {
        char dataPacket[TFTP_BLOCK_SIZE + 4]; // Opcode (2 bytes) + Block Number (2 bytes) + DATA
        dataPacket[0] = (TFTP_OPCODE_DATA >> 8) & 0xFF;
        dataPacket[1] = TFTP_OPCODE_DATA & 0xFF;

        // Block Number
        dataPacket[2] = (blockNumber >> 8) & 0xFF;
        dataPacket[3] = blockNumber & 0xFF;

        // Read data from the file
        size_t bytesRead = read(filefd, &dataPacket[4], TFTP_BLOCK_SIZE);
        print_message("test\n");
        write(STDOUT_FILENO, dataPacket, bytesRead + 4);
        print_message("test\n");

        // Send the DATA packet
        ssize_t bytesSent = send(sockfd, dataPacket, bytesRead + 4, 0);
        if (bytesSent == -1) {
            print_error("Error sending DATA packet");
            close(filefd);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // Receive ACK for the sent block
        int receivedBlockNumber = receiveTFTPAckPacket(sockfd);

        if (receivedBlockNumber == blockNumber) {
            // Increment block number for the next iteration
            blockNumber++;

            // Check if the last block has been sent
            if (bytesRead < TFTP_BLOCK_SIZE) {
                break;
            }
        } else {
            // Handle error (e.g., retransmit or abort)
            print_message("Error: Unexpected ACK received.\n");
            close(filefd);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    print_message("File sent successfully.\n");

    // Close the file
    close(filefd);
}

void gettftp(char * host, char * file) {
    file = file; // tmp 
    void * addr = get_ip(host);
    char * str_addr = str_ip(addr);
    int sockfd = get_socket(str_addr);

    if (DEBUG) print_message("Connected\n");

    sendTFTPReadRequest(file, sockfd);
    receivePacket(sockfd, file);


    close(sockfd); 
      
}
 
void puttftp(char * host, char * file) {
    file = file; // tmp 
    void * addr = get_ip(host);
    char * str_addr = str_ip(addr);
    int sockfd = get_socket(str_addr);

    if (DEBUG) print_message("Connected\n");

    sendTFTPWriteRequest(file, sockfd);

    receiveTFTPAckWRQ(sockfd);

    writePacket(sockfd, file);


    close(sockfd); 
} 