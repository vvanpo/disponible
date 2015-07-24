#ifndef SELF_H
#define SELF_H

/// includes
#include <arpa/inet.h>
#include <openssl/ripemd.h>
#include <pthread.h>
#include <stdbool.h>

/// macro definitions
#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH
#define KEY_LENGTH 256
#define HMAC_KEY_LENGTH 16
#define MAX_UDP_PAYLOAD 512
#define DEFAULT_BUCKET_SIZE 20
#define DEFAULT_BUCKET_MAX_DEPTH 4

/// type definitions
typedef unsigned char byte;
typedef struct {
    byte *data;
    unsigned int length;
} buffer;
//TODO: try using some preprocessor magic to make most hashes static arrays
typedef unsigned char * hash;
struct self {
    struct {
        char *file_folder;
        int udp_port;
        int tcp_port;
        int bucket_size;
        int bucket_max_depth;
    } config;
    hash fingerprint;
    buffer public_key;
    buffer private_key;
    buffer *authorized_keys;
    struct peers *peers;
    struct files *files;
    struct {
        struct message *head;
        struct message *tail;
        pthread_mutex_t mutex;
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
    hash fingerprint;
    byte public_key[KEY_LENGTH];
    struct address address;
    // current sequence number in communication with this peer
    uint32_t sequence_no;
    byte hmac_key[HMAC_KEY_LENGTH];
    //TODO: known file list for this node
    struct bucket *bucket;
    struct peer *next;
    struct peer *prev;
    pthread_mutex_t mutex;
};
enum message_type { find_peer, find_file, store_ref, command, resend,
        NUM_MESSAGE_TYPES };
enum message_class { request, response };
union request {
    // find_peer requests information about a specific fingerprint
    // find_peer is sent to the peer with the given fingerprint on first
    // communication
    struct {
        hash fingerprint;
        // if sequence_no == 0
        byte public_key[KEY_LENGTH];
    } find_peer;
    // find_file includes a file's hash, and returns a list of nearby peers,
    // or if the requestee knows owners of the file, a list of owners along
    // with more nearby peers if there is room left in list_max
    // if find_file is sent to an owner of the file, the owner responds
    // with a list of more owners/nearby peers just like above, but also
    // sets up a bulk file transfer to begin download of the file by the
    // requester
    struct { hash file_hash; } find_file;
    // store_ref is used request to a peer nearby a given file hash to store
    // this node's fingerprint/file hash pair in their remote file table
    // optionally, the requestee can forward the fingerprint/hash pair to
    // any peers it knows even closer to the file
    struct {
        hash file_hash;
        // file_owner field used when forwarding
        hash file_owner;
    } store_ref;
    // command is used by a client, and is always encrypted
    struct {} command;
    // resend is request only
    // if a node receives a message from an unknown peer and the sequence_no of
    // the message is not 0, then it is likely that the node removed said peer
    // from its list
    // to add the peer back to its list, the node can request a resend for
    // sequence_no 0, which would indicate to the peer to restart the key
    // exchange
    struct { uint32_t sequence_no; } resend;
};
union response {
    // find_peer replies with a list of peers close to the requested, or a table
    // of info about the peer
    // when a node sends a find_peer request with a fingerprint request of the
    // same peer it is being sent to, the peer responds with its public key, a
    // signature of its fingerprint to authenticate, and an encrypted shared key
    // to be used hmac (the initial request included the requester's public key)
    // all subsequent messages are authenticated by hmac
    struct {
        struct peer *list;
        // on first commmunication
        byte shared_key[HMAC_KEY_LENGTH];
        byte public_key[KEY_LENGTH];
        byte signature[KEY_LENGTH];
    } find_peer;
    struct {} find_file;
    struct {} store_ref;
    struct {} command;
};
struct message {
    struct peer *peer;
    struct address address;
    hash hmac;
    // used for received messages
    bool hmac_valid;
    // a given sequence_no uniquely identifies a message for that peer
    // note that since udp is unreliable, not all sequence numbers will be
    // received, and they won't necessarily be in order
    // the number can roll over, hence not truly unique
    // further, sequence_no 0 and 1 are always a find_peer request/response pair
    // for the purposes of key exchange (although the response can take more
    // than one datagram)
    uint32_t sequence_no;
    enum message_class class;
    enum message_type type;
    hash fingerprint;
    union {
        union request request;
        union response response;
    };
    // for message queue
    struct message *next;
};
typedef struct files files;

/// extern function declarations
files * file_create_list();
void    file_read_table(files *);
hash    hash_digest(buffer);
hash    hash_file_digest(char *);
char *  hash_base64_encode(hash);
hash    hash_copy(hash);
int     hash_cmp(hash, hash);
void    hash_distance(hash, hash, hash);
void *  message_recv(void *);
struct  message *message_parse(struct self *, buffer);
void    message_enqueue_recv(struct self *, struct message *);
struct  peers *peer_create_list();
void    peer_read_table(struct peers *);
struct  peer *peer_find(struct peers *, hash);
struct  peer *peer_add(struct peers *, hash);
void    remove_peer(struct peers *, struct peer *);
struct  self *self_load();
void    self_run_daemon(struct self *);
buffer  read_file(char *);
void    write_file(char *, buffer);
char *  util_base64_encode(buffer);
buffer  util_base64_decode(char *);
void    util_get_address(struct address *, struct sockaddr *);

#endif
