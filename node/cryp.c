#include <assert.h>
#include <gcrypt.h>
#include <stdlib.h>

/// implementing header
#include "cryp.h"

struct keys {
	gcry_sexp_t *data;
};

// gcry_create_nonce(buf, len);

inline void cryp_hash(unsigned char *out, void *in, int len)
{
	gcry_md_hash_buffer(GCRY_MD_SHA256, out, in, len);
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

int cryp_gen_keys(struct keys *keys)
{
	gcry_sexp_t *param = NULL;
	char *sexp = "(genkey (rsa (nbits 4:2048) (flags oaep pss)))";
	assert(!gcry_sexp_new(param, sexp, strlen(sexp), 1));
	gcry_error_t err = gcry_pk_genkey(keys->data, *param);
	if (err) return -1; // system error
	gcry_sexp_release(*param);
	return 0;
}
