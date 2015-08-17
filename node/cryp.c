#include <assert.h>
#include <ctype.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

/// implementing header
#include "cryp.h"

inline void cryp_hash(unsigned char *out, void const *in, size_t len)
{
	SHA256(in, len, out);
}

inline void cryp_hash_cmp(int *ret, unsigned char const *h1,
	unsigned char const *h2)
{
	*ret = memcmp(h1, h2, HASH_LEN);
}

// cryp_hash_dist calculates the distance metric of 'in' and 'out' and stores in
//   'out'.
void cryp_hash_dist(unsigned char *out, unsigned char const *in)
{
	for (int i = 0; i < HASH_LEN; i++) out[i] ^= in[i];
}

// cryp_b64_encode takes binary input and writes the base64-encoded string
//   to 'out'. Use the macro CRYP_B64_ENCODE_LEN(x) to determine the needed
//   length for 'out'.
void cryp_b64_encode(char *out, void const *in, size_t len)
{
	int j = 0;
	for (int i = 0; i < len; i += 3) {
		uint32_t grp = ((char *) in)[i] << 16;
		if (len - i > 1) grp += ((char *) in)[i + 1] << 8;
		if (len - i > 2) grp += ((char *) in)[i + 2];
		for (int k = 0; k < 4; k++) {
			int n = (grp >> (18 - k * 6)) & 0x3f;
			unsigned char c;
			if (n <= 25) c = n + 'A';
			else if (n <= 51) c = n - 26 + 'a';
			else if (n <= 61) c = n - 52 + '0';
			else if (n == 62) c = '+';
			else c = '/';
			if (len == i + 1 && k >= 2 || len == i + 2 && k == 3)
				out[j] = '=';
			else out[j] = c;
			j++;
		}
	}
	out[j] = '\0';
}

// cryp_b64_decode writes to 'out' the binary data decoded from 'in'. Use the
//   macro CRYP_B64_DECODE_LEN(strlen(in)) to determine a safe length for 'out'.
//   The number of bytes written is stored in *len if 'len' is non-null.
//   On error, returns:
//     ERR_CRYP_B64_INVALID
int cryp_b64_decode(void *out, size_t *len, char const *in)
{
	int l = strlen(in);
	if (l % 4) return ERR_CRYP_B64_INVALID;
	int j = 0;
	for (int i = 0; i < l; i += 4) {
		uint32_t grp = 0;
		int n, c;
		for (int k = 0; k < 4; k++) {
			c = (int) in[i + k];
			if (isupper(c)) n = c - 'A';
			else if (islower(c)) n = c - 'a' + 26;
			else if (isdigit(c)) n = c - '0' + 52;
			else if (c == '+') n = 62;
			else if (c == '/') n = 63;
			else if (c == '=') break;
			else return ERR_CRYP_B64_INVALID;
			grp += n << (18 - k * 6);
		}
		int lim = 3;
		if (c == '=') lim = l == i + 1 ? 1 : 2;
		for (int k = 0; k < lim; k++) {
			((char *) out)[j] = (0xff & (grp >> (16 - k * 8)));
			j++;
		}
	}
	if (len) *len = j;
	return 0;
}

// cryp_gen_keypair generates a new private/public key pair.
int cryp_gen_keypair(void **keypair)
{
	*keypair = RSA_generate_key(KEY_MOD_LEN * 8, 65537, NULL, NULL);
	if (!*keypair) return ERR_CRYP_LIBCRYPTO;
	return 0;
}

// cryp_pub_key_encode writes an encoded form of 'key' to 'out', which must be
//   of length PUB_KEY_LEN.
void cryp_pub_key_encode(void *out, void const *key)
{
	memset(out, 0, PUB_KEY_LEN);
	RSA const *rsa = key;
	assert(rsa->e && rsa->n);
	BN_bn2bin(rsa->e, out);
	BN_bn2bin(rsa->n, out + 4);
}

// cryp_pub_key_decode decodes a public key into an internal representation.
void cryp_pub_key_decode(void **key, void const *in)
{
	RSA *rsa = RSA_new();
	rsa->e = BN_bin2bn(in, 4, NULL);
	rsa->n = BN_bin2bn(in + 4, KEY_MOD_LEN, NULL);
	*key = rsa;
}
