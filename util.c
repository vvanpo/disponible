///implementing header
#include "util.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "hash.h"

// distance computes the distance metric between 2 keys
byte *distance(byte *a, byte *b){
    byte *d = malloc(DIGEST_LENGTH);
    for (int i = 0; i < DIGEST_LENGTH; i++){
        d[i] = a[i] ^ b[i];
    }
    return d;
}

// read_file returns the given file in a buffer, and updates l
byte *read_file(char *path, int *l){
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    byte *buf = NULL;
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
