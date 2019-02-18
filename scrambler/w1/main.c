#include <stdio.h>
#include "scrambler.h"

int main(int argc, char **argv) {
    char *sig = scrambler_get_string(IG_KEY);
    if (sig == NULL) {
        printf("sig == NULL");
        return -1;
    } else {
        printf("%s", sig);
        return 0;
    }
}