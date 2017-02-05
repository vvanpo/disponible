#include <nacl/crypto_hash_sha256.h>

#include "self.h"

void hash (unsigned char *out, void const *in, size_t len)
{
    crypto_hash_sha256(out, in, len);
}

void base64_encode (char *out, void const *in, size_t len)
{
    
}

int base64_decode (void *out, size_t *len, char const *in)
{
    
}

void new_keypair (unsigned char *public, unsigned char *private)
{
    crypto_box_keypair(private, public);
}
