#include <assert.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <stdlib.h>
#include <string.h>

/// implementing header
#include "cryp.h"

inline void cryp_hash(unsigned char *out, void *in, size_t len)
{
	SHA256(in, len, out);
}

inline void cryp_hash_cmp(int *ret, unsigned char *h1, unsigned char *h2)
{
	*ret = memcmp(h1, h2, HASH_LEN);
}

void cryp_hash_dist(unsigned char *dist, unsigned char *h1, unsigned char *h2)
{
	for (int i = 0; i < HASH_LEN; i++)
		dist[i] = h1[i] ^ h2[i];
}

int cryp_gen_key_pair(void **key_pair)
{
	RSA *key = RSA_generate_key(2048, 65537, NULL, NULL);
	if (!key) return ERR_CRYP_LIBCRYPTO;
	*key_pair = key;
	return 0;
}
