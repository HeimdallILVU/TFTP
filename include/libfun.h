#define NOT_ENOUGH_ARGS "Not enough args"
#define TOO_MANY_ARGS "Too many args"
#define GET_ADDR_INFO_ERROR "Erreur lors de la résolution de l'adresse"
#define SOCKET_CONNECT_ERROR "Error connecting"
#define SOCKET_CREATE_ERROR "Error Creating socket"
#define IP_INVALID_ERROR "Invalid IP address"

#define PORT "69"

void print_message(char * message);
void print_error(char * error);

void gettftp(char * host, char * file);
void puttftp(char * host, char * file);