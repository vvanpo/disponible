/// implementing header
#include "self.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// hash_digest writes the message digest to 'out', as computed from 'in'
// out must be at least of length DIGEST_LENGTH
void hash_digest(byte *out, byte *in, int length){
    assert(in && out);
    RIPEMD160(in, length, out);
}

// hash_file_digest computes the message digest of the file at the given path
void hash_file_digest(byte *hash, char *path){
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    RIPEMD160_CTX c;
    if (!RIPEMD160_Init(&c)); //error
    byte *buf = malloc(4096);
    if (!buf); //error
    int ret;
    do {
        ret = read(fd, buf, 4096);
        if (!RIPEMD160_Update(&c, buf, ret)); //error
    } while (ret > 0);
    if (ret == -1); //error
    free(buf);
    if (!RIPEMD160_Final(hash, &c)); //error
}

// hash_base64_encode writes base64-encoded values as per rfc4648 to 'out'
// out must be at least B64_DIGEST_LENGTH bytes
void hash_base64_encode(char *out, byte *hash){
    char *tmp = util_base64_encode(hash, DIGEST_LENGTH);
    memcpy(out, tmp, B64_DIGEST_LENGTH);
    free(tmp);
}

// hash_cmp returns returns < 0, 0, or > 0 if h1 is closer than, the same, or
// further than h2 according to the distance metric
int hash_cmp(byte *h1, byte *h2){
    return memcmp(h1, h2, DIGEST_LENGTH);
}

// hash_distance computes the distance metric (i.e. XOR between hashes) between
// 2 keys and stores it in dist
// dist needs to be pre-allocated, or it can optionally be oneof h1 or h2
void hash_distance(byte *dist, byte *h1, byte *h2){
    for (int i = 0; i < DIGEST_LENGTH; i++)
        dist[i] = h1[i] ^ h2[i];
}
