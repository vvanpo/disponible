///implementing header
#include "self.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// read_file returns the given file in a buffer
// the returned buffer is null-terminated in case the output is to be
// interpreted as text, such that buf.data[buf.length] == '\0',
// i.e. buf.length is not incrememnted to include the NULL byte
buffer read_file(char *path){
    buffer buf = { NULL, 0 };
    if (access(path, R_OK)){
        if (errno != ENOENT); //error
        return buf;
    }
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    int ret;
    do {
        buf.data = realloc(buf.data, buf.length + 513);
        if (!buf.data); //error
        buf.data[buf.length + 512] = '\0';
        ret = read(fd, buf.data + buf.length, 512);
        if (ret == -1) break; //error
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
}

// util_base64_encode outputs base64-encoded values as per rfc4648
char *util_base64_encode(buffer buf){
    // output buffer needs to be divisible by 4, plus one extra byte for null-
    // termination
    int length = (4 * ((buf.length + 2) / 3)) + 1;
    char *out = malloc(length);
    if (!out); //error
    out[length - 1] = '\0';
    for (int i = 0, j = 0; i < buf.length; i += 3){
        unsigned int grp =  buf.data[i] << 16;
        if (buf.length - i > 1) grp += buf.data[i + 1] << 8;
        if (buf.length - i > 2) grp += buf.data[i + 2];
        for (int k = 0; k < 4; k++){
            int n = (grp & (0x3f << (18 - k * 6))) >> (18 - k * 6);
            byte c;
            if (n <= 25) c = n + 'A';
            else if (n <= 51) c = n - 26 + 'a';
            else if (n <= 61) c = n - 52 + '0';
            else if (n == 62) c = '+';
            else c = '/';
            if (((buf.length - i == 1) && k >= 2) ||
                    ((buf.length - i == 2) && k == 3))
                out[j + k] = '=';
            else
                out[j + k] = c;
        }
        j += 4;
    }
    return out;
}

// util_base64_decode takes base64-encoded values as per rfc4648, and returns
// the decoded output
//TODO: does not handle concatenated base64 streams delineated with padding
// characters
buffer util_base64_decode(char *str){
    buffer out = { NULL, 0 };
    if (!str) return out;
    buffer in = { (byte *) str, strlen(str) };
    if (in.length % 4); //error invalid format
    // determine length of output buffer first
    out.length = 3 * in.length / 4;
    if (in.data[in.length - 1] == '=') out.length -= 1;
    if (in.data[in.length - 2] == '=') out.length -= 1;
    out.data = malloc(sizeof(byte) * out.length);
    if (!out.data); //error
    for (int i = 0, j = 0; i < in.length; i += 4){
        int grp = 0;
        for (int k = 0; k < 4; k++){
            int n, c = (int) in.data[i + k];
            if ('A' <= c && c <= 'Z') n = c - 'A';
            else if ('a' <= c && c <= 'z') n = c - 'a' + 26;
            else if ('0' <= c && c <= '9') n = c - '0' + 52;
            else if (c == '+') n = 62;
            else if (c == '/') n = 63;
            else if (c == '=') break;
            else ; //error invalid character
            grp += n << (18 - k * 6);
        }
        for (int k = 0; k < 3 && k + j < out.length; k++){
            out.data[k + j] = (byte) (0xff & (grp >> (16 - k * 8)));
        }
        j += 3;
    }
    return out;
}

// util_get_address transforms a sockaddr structure into an address structure
void util_get_address(struct address *a, struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        a->ip_version = ipv4;
        a->udp_port = ntohl(((struct sockaddr_in *) sa)->sin_port);
        memcpy(a->ip, &((struct sockaddr_in *) sa)->sin_addr.s_addr, 4);
    }
    if (sa->sa_family == AF_INET6){
        a->ip_version = ipv6;
        a->udp_port = ntohl(((struct sockaddr_in6 *) sa)->sin6_port);
        memcpy(a->ip, ((struct sockaddr_in6 *) sa)->sin6_addr.s6_addr, 16);
    }
}

char *util_get_fqdn(struct sockaddr *sa, socklen_t salen){
    char *host = malloc(51);
    int ret = getnameinfo(sa, salen, host, 50, NULL, 0, 0);
    if (ret); //error (gai_strerror)
    return host;
}
