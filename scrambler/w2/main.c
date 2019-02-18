#include <stdio.h>
#include "scrambler.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("no key passed");
        return -1;
    }
    char *sig = scrambler_get_string(argv[1]);
    if (sig == NULL) {
        printf("sig == NULL");
        return -1;
    } else {
        printf("%s", sig);
        return 0;
    }
}