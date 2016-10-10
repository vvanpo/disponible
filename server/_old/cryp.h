#ifndef CRYP_H
#define CRYP_H

#include <stddef.h>

#define HASH_LEN 32
#define ASYM_KEY_LEN 256
#define SYM_KEY_LEN 16

#define CRYP_B64_ENCODED_SIZE(l) (4 * (((l) + 2) / 3) + 1)
#define CRYP_B64_DECODED_SIZE(l) (3 * ((l) / 4))

void cryp_hash(unsigned char *out, void const *in, size_t len);
void cryp_hash_cmp(int *ret, unsigned char const *h1, unsigned char const *h2);
void cryp_hash_dist(unsigned char *out, unsigned char const *in);

void cryp_b64_encode(char *out, void const *in, size_t len);
int cryp_b64_decode(void *out, size_t *len, char const *in);

int cryp_gen_keypair(void **keypair);
void cryp_pub_key_encode(unsigned char *out, void const *key);
void cryp_pub_key_decode(void **key, unsigned char const *in);

#endif
