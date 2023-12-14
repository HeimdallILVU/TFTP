#include "libfun.h"


int main(int argc, char * argv[]) {
    if(argc == 0) {
        print_error(NOT_ENOUGH_ARGS);
    }

    if(strcmp(argv[1], "gettftp") == 0) {
        gettftp(argv[2], argv[3]);
    }

    if(strcmp(argv[1], "puttftp") == 0) {
        puttftp(argv[2], argv[3]);
    }

    return 0;
}
