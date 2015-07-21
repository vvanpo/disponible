/// implementing header
#include "hash.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// hash_digest takes a reference to an already-allocated hash, and
// populates it with the message digest computed from data in buf
void hash_digest(hash hash, void *buf, int buf_length){
    RIPEMD160((unsigned char *) buf, buf_length, hash);
}

// hash_file_digest computes the message digest of the file at the given path
// returns 0 on success, -1 on error
int hash_file_digest(hash hash, char *path){
    struct stat st;
    if (stat(path, &st)); //error
    if (!S_ISREG(st.st_mode)); //error
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    RIPEMD160_CTX c;
    if (!RIPEMD160_Init(&c)); //error
    void *buf = malloc(4096);
    if (!buf); //error
    int ret;
    do {
        ret = read(fd, buf, 4096);
        if (!RIPEMD160_Update(&c, buf, ret)); //error
    } while (ret > 0);
    if (ret == -1); //error
    free(buf);
    if (!RIPEMD160_Final(hash, &c)); //error
    return 0;
}

// hash_base64_encode outputs base64-encoded values as per rfc4648
char *hash_base64_encode(void *buf, int buf_length){
    // output buffer needs to be divisible by 4, plus one extra byte for null-
    // termination
    int length = (4 * ((buf_length + 2) / 3)) + 1;
    char *out = malloc(length);
    if (!out); //error
    out[length - 1] = '\0';
    for (int i = 0, j = 0; i < buf_length; i += 3){
        unsigned int grp = ((unsigned char *) buf)[i] << 16;
        if (buf_length - i > 1) grp += ((unsigned char *) buf)[i + 1] << 8;
        if (buf_length - i > 2) grp += ((unsigned char *) buf)[i + 2];
        for (int k = 0; k < 4; k++){
            int n = (grp & (0x3f << (18 - k * 6))) >> (18 - k * 6);
            unsigned char c;
            if (n <= 25) c = n + 'A';
            else if (n <= 51) c = n - 26 + 'a';
            else if (n <= 61) c = n - 52 + '0';
            else if (n == 62) c = '+';
            else c = '/';
            if (((buf_length - i == 1) && k >= 2) ||
                    ((buf_length - i == 2) && k == 3))
                out[j + k] = '=';
            else
                out[j + k] = c;
        }
        j += 4;
    }
    return out;
}

// hash_base64_decode takes base64-encoded values as per rfc4648, and returns
// the decoded output as well as updating the out_length reference
//TODO: does not handle concatenated base64 streams delineated with padding
// characters
unsigned char *hash_base64_decode(char *buf, int *out_length){
    // determine length of output buffer first
    int buf_length = strlen(buf);
    if (buf_length % 4); //error invalid format
    *out_length = 3 * buf_length / 4;
    if (!buf_length) return NULL;
    if (buf[buf_length - 1] == '=') *out_length -= 1;
    if (buf[buf_length - 2] == '=') *out_length -= 1;
    unsigned char *out = malloc(sizeof(unsigned char) * *out_length);
    for (int i = 0, j = 0; i < buf_length; i += 4){
        int grp = 0;
        for (int k = 0; k < 4; k++){
            int n, c = (int) buf[i + k];
            if ('A' <= c && c <= 'Z') n = c - 'A';
            else if ('a' <= c && c <= 'z') n = c - 'a' + 26;
            else if ('0' <= c && c <= '9') n = c - '0' + 52;
            else if (c == '+') n = 62;
            else if (c == '/') n = 63;
            else if (c == '=') break;
            else ; //error invalid character
            grp += n << (18 - k * 6);
        }
        for (int k = 0; k < 3 && k + j < *out_length; k++){
            out[k + j] = (unsigned char) (0xff & (grp >> (16 - k * 8)));
        }
        j += 3;
    }
    return out;
}

// hash_digest_base64 returns the base64 encoded hash value
char *hash_digest_base64(hash hash){
    return hash_base64_encode(hash, DIGEST_LENGTH);
}
