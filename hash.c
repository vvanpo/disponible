/// implementing header
#include "hash.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// hash_digest populates the passed hash with the message digest computed from
// the data in the passed buffer
hash hash_digest(buffer buf){
    hash hash = malloc(DIGEST_LENGTH);
    if (!hash); //error
    // RIPEMD160 doesn't specify how it handles errors
    if (!buf.data); //error
    RIPEMD160(buf.data, buf.length, hash);
    return hash;
}

// hash_file_digest computes the message digest of the file at the given path
hash hash_file_digest(char *path){
    //TODO: handle ENOENT
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    hash hash = malloc(DIGEST_LENGTH);
    if (!hash); //error
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
    return hash;
}

// hash_base64_encode outputs base64-encoded values as per rfc4648
char *hash_base64_encode(hash hash){
    buffer buf = { hash, DIGEST_LENGTH };
    return util_base64_encode(buf);
}

// hash_copy allocates a new hash and copies in the contents of the passed hash
hash hash_copy(hash in){
    hash out = malloc(DIGEST_LENGTH);
    if (!out); //error
    memcpy(out, in, DIGEST_LENGTH);
    return out;
}

// hash_cmp returns returns < 0, 0, or > 0 if h1 is closer than, the same, or
// further than h2 according to the distance metric
int hash_cmp(hash h1, hash h2){
    return memcmp(h1, h2, DIGEST_LENGTH);
}

// hash_distance computes the distance metric (i.e. XOR between hashes) between
// 2 keys and stores it in dist
void hash_distance(hash dist, hash a, hash b){
    for (int i = 0; i < DIGEST_LENGTH; i++)
        dist[i] = a[i] ^ b[i];
}
