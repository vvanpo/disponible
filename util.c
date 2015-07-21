///implementing header
#include "util.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// read_file returns the given file in a buffer
// the returned buffer is null-terminated in case the output is to be
// interpreted as text, such that buf.data[buf.length] == '\0',
// i.e. buf.length is not incrememnted to include the NULL byte
buffer read_file(char *path){
    //TODO: handle ENOENT
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    buffer buf = { NULL, 0 };
    int ret;
    do {
        buf.data = realloc(buf.data, buf.length + 513);
        buf.data[buf.length + 512] = '\0';
        ret = read(fd, buf.data + buf.length, 512);
        if (ret == -1); //error
        buf.length += ret;
    } while (ret);
    if (close(fd)); //error
    return buf;
}

// write_file atomically creates or overwrites the given file with the data in
// the passed buffer
void write_file(char *path, buffer buf){
    char *tmp = calloc(1, strlen(path) + 1);
    strcat(tmp, "~");
    if (unlink(tmp) == -1)
        if (errno != ENOENT); //error
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1); //error
    //TODO: account for partial writes
    int ret = write(fd, buf.data, buf.length);
    if (ret == -1); //error
    if (ret < buf.length); //error
    if (fsync(fd)); //error
    if (close(fd)); //error
    if (rename(tmp, path)); //error
    free(tmp);
    return 0;
}
