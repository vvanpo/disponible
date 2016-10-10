#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <libdsp.h>

#define HASH_LEN DSP_HASH_LEN

void hash (unsigned char *out, void const *in, size_t len);
void encode_b64 (char *out, void const *in, size_t len);
int decode_b64 (void *out, size_t *len, char const *in);
int generate_keys (void **keys);

#endif
