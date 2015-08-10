///implementing header
#include "self.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// util_read_file allocates and writes the given file to '*out'
// returns the number of bytes written, excluding NULL-terminator
// i.e. *out[read_file(out, path)] == '\0'
// returns 0 if the file could not be opened, due to permissions or not
// existing, or the file was empty
// if the file was empty (*out)[0] == '\0', whereas if it did not exist *out == NULL
int util_read_file(byte **out, char *path){
    if (access(path, R_OK)){
        if (errno == ENOENT || errno == EACCES || errno == ENOTDIR)
            return 0;
        //error
    }
    int fd = open(path, O_RDONLY);
    if (fd == -1); //error
    int ret, length = 0;
    do {
        *out = realloc(*out, length + 513);
        if (!*out); //error
        ret = read(fd, *out + length, 512);
        if (ret == -1) break; //error
        (*out)[length + ret] = '\0';
        length += ret;
    } while (ret);
    if (close(fd)); //error
    return length;
}

// util_write_file atomically creates or overwrites the given file with the data
// in 'in' of 'length' bytes
void util_write_file(char *path, byte *in, int length){
    // + 2 to account for NULL-termination
    char *tmp = calloc(1, strlen(path) + 2);
    strcpy(tmp, path);
    strcat(tmp, "~");
    if (unlink(tmp) == -1)
        if (errno != ENOENT); //error
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1); //error
    //TODO: account for partial writes
    int ret = write(fd, in, length);
    if (ret == -1); //error
    if (ret < length); //error
    if (fsync(fd)); //error
    if (close(fd)); //error
    if (rename(tmp, path)); //error
    free(tmp);
}

// util_base64_encode writes base64-encoded values as per rfc4648 to 'out'
// 'out' must be of at least util_base64_encoded_size(length) + 1 bytes (for
// NULL-termination)
void util_base64_encode(char *out, byte *in, int length){
    out[util_base64_encoded_size(length)] = '\0';
    for (int i = 0, j = 0; i < length; i += 3){
        unsigned int grp =  in[i] << 16;
        if (length - i > 1) grp += in[i + 1] << 8;
        if (length - i > 2) grp += in[i + 2];
        for (int k = 0; k < 4; k++){
            int n = (grp & (0x3f << (18 - k * 6))) >> (18 - k * 6);
            byte c;
            if (n <= 25) c = n + 'A';
            else if (n <= 51) c = n - 26 + 'a';
            else if (n <= 61) c = n - 52 + '0';
            else if (n == 62) c = '+';
            else c = '/';
            if (((length - i == 1) && k >= 2) ||
                    ((length - i == 2) && k == 3))
                out[j + k] = '=';
            else
                out[j + k] = c;
        }
        j += 4;
    }
}

// util_base64_decode takes a base64-encoded value as per rfc4648, and writes
// the decoded output to 'out'
// 'out' must be of at least util_base64_decoded_size(in) bytes
//TODO: does not handle concatenated base64 streams delineated with padding
// characters
void util_base64_decode(byte *out, char *in){
    assert(out && in);
    int length = strlen(in);
    for (int i = 0, j = 0; i < length; i += 4){
        int grp = 0;
        for (int k = 0; k < 4; k++){
            int n, c = (int) in[i + k];
            if ('A' <= c && c <= 'Z') n = c - 'A';
            else if ('a' <= c && c <= 'z') n = c - 'a' + 26;
            else if ('0' <= c && c <= '9') n = c - '0' + 52;
            else if (c == '+') n = 62;
            else if (c == '/') n = 63;
            else if (c == '=') break;
            else ; //error invalid character
            grp += n << (18 - k * 6);
        }
        for (int k = 0; k < 3 && k + j < util_base64_decoded_size(in); k++){
            out[k + j] = (byte) (0xff & (grp >> (16 - k * 8)));
        }
        j += 3;
    }
}

// util_base64_encoded_size returns the number of chars it would take to encode
// a buffer of size 'length'
int util_base64_encoded_size(int length){
    return 4 * ((length + 2) / 3);
}

// util_base64_decoded_size returns the number of bytes it would take to decode
// 'in' (base64 strings need to be divisible by 4)
int util_base64_decoded_size(char *in){
    int length = strlen(in);
    if (!length) return 0;
    assert(length % 4 == 0); // invalid format
    int l = 3 * length / 4;
    if (in[length - 1] == '=') l -= 1;
    if (in[length - 2] == '=') l -= 1;
    return l;
}

// util_get_address transforms a sockaddr structure into an address structure
void util_get_address(struct address *a, struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        a->ip_version = ipv4;
        a->udp_port = ntohl(((struct sockaddr_in *) sa)->sin_port);
        memcpy(a->ip, &((struct sockaddr_in *) sa)->sin_addr.s_addr, 4);
    }
    if (sa->sa_family == AF_INET6){
        a->ip_version = ipv6;
        a->udp_port = ntohl(((struct sockaddr_in6 *) sa)->sin6_port);
        memcpy(a->ip, ((struct sockaddr_in6 *) sa)->sin6_addr.s6_addr, 16);
    }
}

// util_get_sockaddr transforms an address structure into a sockaddr structure
struct sockaddr *util_get_sockaddr(struct sockaddr_storage *sa,
        struct address *a){
    if (a->ip_version == ipv4){
        struct sockaddr_in *s = (struct sockaddr_in *) sa;
        s->sin_family = AF_INET;
        memcpy(&s->sin_addr.s_addr, a->ip, 4);
        s->sin_port = htonl(a->udp_port);
    }
    if (a->ip_version == ipv6){
        struct sockaddr_in6 *s = (struct sockaddr_in6 *) sa;
        s->sin6_family = AF_INET6;
        memcpy(s->sin6_addr.s6_addr, a->ip, 16);
        s->sin6_port = htonl(a->udp_port);
    }
    return (struct sockaddr *) sa;
}

// util_get_fqdn returns the fqdn of address specified in 'sa'
char *util_get_fqdn(struct sockaddr *sa, socklen_t salen){
    char *host = malloc(51);
    int ret = getnameinfo(sa, salen, host, 50, NULL, 0, 0);
    if (ret); //error (gai_strerror)
    return host;
}

// util_parse_address takes an address of the form (fqdn|ip):port and stores it
// in the address structure 'a'
void util_parse_address(struct address *a, char *s){
    int l = strlen(s);
    char *tmp;
    if (*s == '['){
        tmp = memchr(s, ']', l);
        if (!tmp || tmp - s == l || *(tmp + 1) != ':'); // user error
        *tmp = '\0';
        if (!inet_pton(AF_INET6, s + 1, a->ip)); // user error
        a->ip_version = ipv6;
        tmp += 2;
    }
    else {
        if (!(tmp = memchr(s, ':', l)) || tmp - s == l); // user error
        *tmp = '\0';
        if (!inet_pton(AF_INET, s, a->ip)){
            if (!(a->fqdn = realloc(a->fqdn, strlen(s) + 1))); // system error
            if (strlen(strcpy(a->fqdn, s)) != tmp - s); // user error
            //TOOD: check fqdn integrity
        }
        tmp++;
    }
    long port = strtol(tmp, &tmp, 10);
    if (*tmp != '\0' || port > 0xffff); // user error
    a->udp_port = (uint16_t) port;
}

// util_hmac_key initializes a new random shared key
void util_hmac_key(byte *key){
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1); //error
    //TODO: loop to make sure short reads don't compromise the key
    int ret = read(fd, key, HMAC_KEY_LENGTH);
    if (ret == -1); //error
    if (close(fd)); //error
}

// util_hmac writes the hmac of 'data' to 'out'
void util_hmac(byte *out, byte *data, int length, byte *key){
    if (!HMAC(EVP_ripemd160(), key, HMAC_KEY_LENGTH, data, length, out, NULL));
        //error
}

// util_read_rsa_pem reads in the PEM private key at path
//TODO: accept (byte *) instead, and detect public or private key
RSA *util_read_rsa_pem(char *path){
    if (access(path, R_OK))
        if (errno != ENOENT); //error
    FILE *fd = fopen(path, "r");
    if (!fd); //error
    RSA *rsa = PEM_read_RSAPrivateKey(fd, NULL, 0, NULL);
    if (!rsa); //error
    if (fclose(fd)); //error
    return rsa;
}

// util_rsa_encrypt encrypts data with the public portion of 'rsa' and writes to
// 'out'
// 'out' must be of at least RSA_MODULUS_LENGTH bytes, and length can't be over
// RSA_MESSAGE_MAX_LENGTH
void util_rsa_encrypt(byte *out, RSA *rsa, byte *data, int length){
    assert(length <= RSA_MESSAGE_MAX_LENGTH);
    if (RSA_public_encrypt(length, data, out, rsa, RSA_PADDING) < 0); //error
}

// util_rsa_decrypt using the private key contained within 'rsa', and writes to
// 'out', with the message length returned
// 'data' must be of at least RSA_MODULUS_LENGTH bytes, and 'out' of at least
// RSA_MESSAGE_MAX_LENGTH bytes
int util_rsa_decrypt(byte *out, RSA *rsa, byte *data){
    assert(out && rsa && data);
    int l =
        RSA_private_decrypt(RSA_MODULUS_LENGTH, data, out, rsa, RSA_PADDING);
    if (l < 0); //error
    return l;
}

// util_rsa_sign signs the message 'm' with the given rsa key
// 'out' must be of RSA_MODULUS_LENGTH bytes
void util_rsa_sign(byte *out, byte *m, int length, RSA *rsa){
    if (length > RSA_MODULUS_LENGTH - 11){
        byte tmp[DIGEST_LENGTH];
        hash_digest(tmp, m, length);
        m = tmp;
        length = DIGEST_LENGTH;
    }
    unsigned int l;
    if (!RSA_sign(NID_ripemd160, m, length, out, &l, rsa));
        //error
}

// util_rsa_verify verifies an RSA signature 's' against message 'm'
// 's' must be RSA_MODULUS_LENGTH bytes
bool util_rsa_verify(byte *s, byte *m, int length, RSA *rsa){
    if (length > RSA_MODULUS_LENGTH - 11){
        byte tmp[DIGEST_LENGTH];
        hash_digest(tmp, m, length);
        m = tmp;
        length = DIGEST_LENGTH;
    }
    if (RSA_verify(NID_ripemd160, m, length, s, RSA_MODULUS_LENGTH, rsa))
        return true;
    return false;  // possible error
}

// util_rsa_pub_encode encodes a public key for transfer
// out must be of at least PUB_KEY_LENGTH bytes
void util_rsa_pub_encode(byte *out, RSA *rsa){
    memset(out, 0, PUB_KEY_LENGTH);
    if (!rsa->e || !rsa->n) assert(false);
    if (BN_bn2bin(rsa->e, out) > RSA_EXPONENT_LENGTH) assert(false);
    if (BN_bn2bin(rsa->n, out + RSA_EXPONENT_LENGTH) != RSA_MODULUS_LENGTH)
        assert(false);
}

// util_rsa_pub_decode takes an array of PUB_KEY_LENGTH bytes and writes its
// values to an RSA object
RSA *util_rsa_pub_decode(byte *in){
    RSA *rsa = RSA_new();
    rsa->e = BN_bin2bn(in, RSA_EXPONENT_LENGTH, NULL);
    rsa->n = BN_bin2bn(in + RSA_EXPONENT_LENGTH, RSA_MODULUS_LENGTH, NULL);
    if (!rsa->e || !rsa->n)
        ERR_print_errors_fp(stderr);
    return rsa;
}
