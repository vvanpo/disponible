#include <assert.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

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

// cryp_base64_encode takes binary input and writes the base64-encoded string
//   to 'out'. Use the macro CRYP_B64_ENCODE_SIZE(x) to determine the needed
//   size for 'out'.
void cryp_base64_encode(char *out, void *in, size_t len)
{
	int j = 0;
	for (int i = 0; i < len; i += 3) {
		uint32_t grp = ((unsigned char *) in)[i] << 16;
		if (len - i > 1) grp += ((unsigned char *) in)[i + 1] << 8;
		if (len - i > 2) grp += ((unsigned char *) in)[i + 2];
		for (int k = 0; k < 4; k++) {
			int n = (grp & (0x3f << (18 - k * 6))) >> (18 - k * 6);
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

// cryp_base64_decode writes to 'out' the binary data decoded from 'in'. Use the
//   macro CRYP_B64_DECODE_SIZE(x) to determine a safe size for 'out'. The
//   number of bytes written is stored in *len.
//   On error, returns:
//     ERR_CRYP_B64_INVALID
int cryp_base64_decode(void *out, size_t *len, char *in)
{
	int l = strlen(in);
	if (l % 4) return ERR_CRYP_B64_INVALID;
	int j = 0;
	for (int i = 0; i < l; i += 4) {
		uint32_t grp = 0;
		int n, c;
		for (int k = 0; k < 4; k++) {
			c = (int) in[i + k];
			if ('A' <= c && c <= 'Z') n = c - 'A';
			else if ('a' <= c && c <= 'z') n = c - 'a' + 26;
			else if ('0' <= c && c <= '9') n = c - '0' + 52;
			else if (c == '+') n = 62;
			else if (c == '/') n = 63;
			else if (c == '=') break;
			else return ERR_CRYP_B64_INVALID;
			grp += n << (18 - k * 6);
		}
		int lim = 3;
		if (c == '=') lim = l == i + 1 ? 1 : 2;
		for (int k = 0; k < lim; k++) {
			((unsigned char *) out)[j] =
				(unsigned char) (0xff & (grp >> (16 - k * 8)));
			j++;
		}
	}
	if (len) *len = j;
	return 0;
}

int cryp_gen_key_pair(void **key_pair)
{
	*key_pair = RSA_generate_key(2048, 65537, NULL, NULL);
	if (!*key_pair) return ERR_CRYP_LIBCRYPTO;
	return 0;
}

