#ifndef SELF_H
#define SELF_H

/// includes
#include <arpa/inet.h>
#include <assert.h>
#include <openssl/ripemd.h>
#include <openssl/rsa.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "disponible.h"

/// macro definitions
#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH
#define B64_DIGEST_LENGTH (4 * ((DIGEST_LENGTH + 2) / 3) + 1)
#define RSA_MODULUS_LENGTH 256
#define RSA_EXPONENT_LENGTH 4
#define PUB_KEY_LENGTH (RSA_EXPONENT_LENGTH + RSA_MODULUS_LENGTH)
#define RSA_PADDING RSA_PKCS1_OAEP_PADDING
#define RSA_MESSAGE_MAX_LENGTH (RSA_MODULUS_LENGTH - 41)
#define HMAC_KEY_LENGTH 16
#define MAX_UDP_PAYLOAD 512
#define DEFAULT_BUCKET_SIZE 20
#define DEFAULT_BUCKET_MAX_DEPTH 4

/// type definitions
typedef unsigned char byte;
struct self {
    struct {
        char *file_folder;
        int udp_port;
        int tcp_port;
        int bucket_size;
        int bucket_max_depth;
    } config;
    byte fingerprint[DIGEST_LENGTH];
    RSA *rsa_key;
    byte **authorized_keys;
    struct peers *peers;
    struct files *files;
    struct {
        struct message *head;
        struct message *tail;
        pthread_mutex_t mutex;
        pthread_cond_t empty;
    } recv_queue;
};
struct address {
    char *fqdn;
    enum { ipv4, ipv6 } ip_version;
    byte ip[16];
    int udp_port;
    int tcp_port;
};
struct peer {
    byte *fingerprint;
    RSA *rsa_public_key;
    struct address address;
    // current sequence number in communication with this peer
    uint32_t sequence_no;
    byte hmac_key[HMAC_KEY_LENGTH];
    //TODO: known file list for this node
    struct peer *next;
    struct peer *prev;
};
enum message_class { request, response };
enum message_type { key_exchange, find_peer, find_file, store_ref, command,
    resend, NUM_MESSAGE_TYPES };
union request {
    // key_exchange is requested on first communication with a peer
    struct { byte public_key[PUB_KEY_LENGTH]; } key_exchange;
    // find_peer requests information about a specific fingerprint
    struct { byte *fingerprint; } find_peer;
    // find_file includes a file's hash, and returns a list of nearby peers,
    // or if the requestee knows owners of the file, a list of owners along
    // with more nearby peers if there is room left in list_max
    // if find_file is sent to an owner of the file, the owner responds
    // with a list of more owners/nearby peers just like above, but also
    // sets up a bulk file transfer to begin download of the file by the
    // requester
    struct { byte *file_hash; } find_file;
    // store_ref is used request to a peer nearby a given file hash to store
    // this node's fingerprint/file hash pair in their remote file table
    // optionally, the requestee can forward the fingerprint/hash pair to
    // any peers it knows even closer to the file
    struct {
        byte *file_hash;
        // file_owner field used when forwarding
        byte *file_owner;
    } store_ref;
    // command is used by a client, and is always encrypted
    struct {} command;
    // resend is request only
    // if a node receives a message from an unknown peer and the sequence_no of
    // the message is not 0, then it is likely that the node removed said peer
    // from its list
    // to add the peer back to its list, the node can request key_exchange
    struct { uint32_t sequence_no; } resend;
};
union response {
    // key_exchange responds with its public key, and an encrypted shared key
    // (encrypted with the requester's public key) to be used for hmac on
    // subsequent messages
    // it immediately sends a second datagram with a signature of its
    // fingerprint, and the hmac for that message
    // the two response datagrams conclude the key exchange protocol, unless a
    // resend is requested
    union {
        struct {
            // encrypted hmac key, same length as RSA key
            byte shared[RSA_MODULUS_LENGTH];
            byte public[PUB_KEY_LENGTH];
        } keys;
        byte signature[RSA_MODULUS_LENGTH];
    } key_exchange;
    // find_peer replies with a list of peers close to the requested, or the
    // address info of the peer
    union {
        // list of fingerprints
        byte **list;
        struct address addr;
    } find_peer;
    struct {} find_file;
    struct {} store_ref;
    struct {} command;
};
struct message {
    struct peer *peer;
    time_t time;
    struct address address;
    byte *hmac;
    // used for received messages
    bool hmac_valid;
    // a given sequence_no uniquely identifies a message for that peer
    // note that since udp is unreliable, not all sequence numbers will be
    // received, and they won't necessarily be in order
    // the number can roll over, hence not truly unique
    uint32_t sequence_no;
    enum message_class class;
    enum message_type type;
    byte *fingerprint;
    union {
        union request request;
        union response response;
    };
    // for message queue
    struct message *next;
};
typedef struct files files;

/// extern function declarations
files *         file_create_list();
void            file_read_table(files *);
void            hash_digest(byte *, byte *, int);
void            hash_file_digest(byte *, char *);
void            hash_base64_encode(char *, byte *);
int             hash_cmp(byte *, byte *);
void            hash_distance(byte *, byte *, byte *);
void            hash_rsa_fingerprint(byte *, RSA *);
struct message *message_parse(struct self *, byte *, int);
void            message_enqueue_recv(struct self *, struct message *);
void *          message_dequeue_recv(void *);
struct message *message_new(enum message_class, enum message_type,
                    struct peer *);
void            message_free(struct message *);
struct peers *  peer_create_list();
void            peer_read_table(struct peers *);
struct peer *   peer_find(struct peers *, byte *);
struct peer *   peer_add(struct peers *, byte *);
void            peer_remove(struct peers *, struct peer *);
void            protocol_key_exchange_recv(struct self *, struct message *);
struct self *   self_load();
void            self_run_daemon(struct self *);
int             util_read_file(byte **, char *);
void            util_write_file(char *, byte *, int);
char *          util_base64_encode(byte *, int);
int             util_base64_decode(byte **, char *);
void            util_get_address(struct address *, struct sockaddr *);
struct sockaddr *util_get_sockaddr(struct sockaddr_storage *, struct address *);
void            util_hmac_key(byte *);
void            util_hmac(byte *, byte *, int, byte *);
RSA *           util_read_rsa_pem(char *);
void            util_rsa_encrypt(byte *, RSA *, byte *, int);
int             util_rsa_decrypt(byte *, RSA *, byte *);
void            util_rsa_pub_encode(byte *, RSA *);
RSA *           util_rsa_pub_decode(byte *);

#endif
