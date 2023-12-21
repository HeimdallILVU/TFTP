#define DEBUG 1

#define WRITE_ERROR "Error writing to file descriptor"
#define NOT_ENOUGH_ARGS "Not enough args"
#define TOO_MANY_ARGS "Too many args"
#define GET_ADDR_INFO_ERROR "Error during address resolution"
#define SOCKET_CONNECT_ERROR "Error connecting"
#define SOCKET_CREATE_ERROR "Error Creating socket"
#define IP_INVALID_ERROR "Invalid IP address"
#define ERROR_SENDING_PACKET "Error sending packet"
#define MESSAGE_SUCCESS_READ_REQUEST "Read request packet sent successfully.\n"
#define MESSAGE_SUCCESS_WRITE_REQUEST "Write request packet sent successfully.\n"
#define MESSAGE_SUCCESS_DATA "Data packet sent successfully.\n"
#define MESSAGE_SUCCESS_ACK "Ack packet sent successfully.\n"
#define READ_REQUEST "\x00\x01heimdall.txt\x00netascii\x00"
#define PORT "69"

#define TFTP_OPCODE_RRQ_READ 1
#define TFTP_OPCODE_RRQ_WRITE 2
#define TFTP_OPCODE_DATA 3
#define TFTP_OPCODE_ACK 4
#define TFTP_BLOCK_SIZE 512
#define TFTP_MODE "netascii"

void print_message(char * message);
void print_error(char * error);

void gettftp(char * host, char * file);
void puttftp(char * host, char * file); 