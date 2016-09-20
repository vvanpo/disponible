#include <stdint.h>
#include <openssl/sha.h>

void hash (unsigned char *out, void const *in, size_t len)
{
    SHA256(in, len, out);
}

void encode_b64 (char *out, void const *in, size_t len)
{
    
}

int decode_b64 (void *out, size_t *len, char const *in)
{
    
}

int generate_keys (void **keys)
{
    *keys = RSA_generate_key(ASYM_KEY_LEN * 8, 65537, NULL, NULL);
    if (!*keys) return -1;
    return 0;
}
