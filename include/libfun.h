#define NOT_ENOUGH_ARGS "Not enough args"
#define TOO_MANY_ARGS "Too many args"
#define GET_ADDR_INFO_ERROR "Erreur lors de la résolution de l'adresse"
#define SOCKET_CONNECT_ERROR "Error connecting"
#define SOCKET_CREATE_ERROR "Error Creating socket"
#define IP_INVALID_ERROR "Invalid IP address"

#define PORT "69"

// TFTP packet types
#define RRQ_OPCODE 1
#define DATA_OPCODE 3
#define ACK_OPCODE 4

void print_message(char * message);
void print_error(char * error);

void gettftp(char * host, char * file);
void puttftp(char * host, char * file);
struct tftp_rrq {
    uint16_t opcode;
    char filename_and_mode[1];  // Variable length (filename + '\0' + mode)
};

struct tftp_data {
    uint16_t opcode;
    uint16_t block_number;
    char data[MAX_BUFFER_SIZE - 4];  // Subtract 4 bytes for opcode and block_number
};

struct tftp_ack {
    uint16_t opcode;
    uint16_t block_number;
};