/// implementing header
#include "self.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "hash.h"

/// type definitions
struct self {
    hash fingerprint;
    unsigned char *public_key;
    unsigned char *private_key;
};

/// static function declarations
//TODO: config file, allowing for alternate key files and the like
//static int read_config();

// read_public_key returns the local public key and length parameter
unsigned char *read_public_key(int *l){
    int fd = open("keys/public", O_RDONLY);
    if (fd == -1); //error
    unsigned char *buf = NULL;
    if (!buf); //error
    int ret;
    *l = 0;
    do {
        buf = realloc(buf, *l + 512);
        ret = read(fd, buf + *l, 512);
        if (ret == -1); //error
        *l += ret;
    } while (ret);
    if (close(fd)); //error
    return buf;
}
