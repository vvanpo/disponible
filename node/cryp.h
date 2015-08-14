#ifndef CRYP_H
#define CRYP_H

#include <stddef.h>

#define HASH_LEN 32

enum cryp_error {
	ERR_CRYP_LIBCRYPTO,
};

void cryp_hash(unsigned char *out, void *in, size_t len);
void cryp_hash_cmp(int *ret, unsigned char *h1, unsigned char *h2);
void cryp_hash_dist(unsigned char *dist, unsigned char *h1, unsigned char *h2);

int cryp_gen_key_pair(void **key_pair);

#endif
