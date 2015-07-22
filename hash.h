#ifndef HASH_H
#define HASH_H

#include "util.h"

/// type definitions
typedef unsigned char* hash;

hash hash_digest(buffer);
hash hash_file_digest(char *);
char *hash_base64_encode(hash);
hash distance(hash, hash);

#endif
