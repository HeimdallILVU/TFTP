#define __USE_POSIX199309 1 // for time.h to define timespec and all
#include <time.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

#include <sys/wait.h>

#include "libfun.h"


void print_message(char * message) {
    write(STDOUT_FILENO, message, strlen(message));
}

void print_error(char * message) {
    write(STDERR_FILENO, message, strlen(message));
}

void gettftp(char * host, char * file) {
    print_message("gettftp test \n");
}

void puttftp(char * host, char * file) {
    print_message("puttftp test \n");
}