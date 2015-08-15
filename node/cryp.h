#ifndef CRYP_H
#define CRYP_H

#include <stddef.h>

#define HASH_LEN 32

#define CRYP_B64_ENCODED_SIZE(x) (4 * (((x) + 2) / 3) + 1)
#define CRYP_B64_DECODED_SIZE(x) (3 * ((x) / 4))

enum cryp_error {
	ERR_CRYP_LIBCRYPTO,
	ERR_CRYP_B64_INVALID,
};

void cryp_hash(unsigned char *out, void *in, size_t len);
void cryp_hash_cmp(int *ret, unsigned char *h1, unsigned char *h2);
void cryp_hash_dist(unsigned char *dist, unsigned char *h1, unsigned char *h2);

void cryp_base64_encode(char *out, void *in, size_t len);
int cryp_base64_decode(void *out, size_t *len, char *in);

int cryp_gen_key_pair(void **key_pair);

#endif
