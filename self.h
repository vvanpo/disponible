#ifndef SELF_H
#define SELF_H

#include <pthread.h>
#include "hash.h"
#include "util.h"

#define KEY_LENGTH 256
#define MAX_UDP_PAYLOAD 512

/// type definitions
struct self {
    struct {
        int udp_port;
        int tcp_port;
        char *file_folder;
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
enum message_type { find_peer, find_file, store_ref, command, resend,
        NUM_MESSAGE_TYPES };

/// extern function declarations
struct self *self_load();
void self_run_daemon(struct self *);
void *message_recv(void *);
struct message *message_parse(buffer);
void message_enqueue_recv(struct self *, struct message *);

#endif
