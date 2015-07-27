/// implementing header
#include "self.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define X(n, s) s,
char *error_str[] = {
    DI_ERRORS
};
#undef X

/// static function declarations
static void load_config(struct self *);
static void self_listen(struct self *);

enum di_error di_daemon_load(struct di_daemon **d, struct di_options *opts){
    struct self *self = self_load();
    *d = (struct di_daemon *) self;
    return DI_ESUCCESS;
}
enum di_error di_daemon_run(struct di_daemon *d){
    self_run_daemon((struct self *) d);
    return DI_ESUCCESS;
}

// self_load loads all the necessary data structures into self required for
// starting the daemon
struct self *self_load(){
    struct self *self = calloc(1, sizeof(struct self));
    if (!self); //error
    load_config(self);
    self->rsa_key = util_read_rsa_pem("keys/private");
    byte tmp[PUB_KEY_LENGTH];
    util_rsa_pub_encode(tmp, self->rsa_key);
    hash_digest(self->fingerprint, tmp, PUB_KEY_LENGTH);
    self->peers = peer_create_list();
    peer_read_table(self->peers);
    //self->files = file_create_list();
    //file_read_table(self->files);
    int err = pthread_mutex_init(&self->recv_queue.mutex, NULL);
    if (err); //error
    err = pthread_cond_init(&self->recv_queue.empty, NULL);
    if (err); //error
    return self;
}

// self_run_daemon starts the main run loop and processing threads
void self_run_daemon(struct self *self){
    pthread_t recv_thread;
    int err = pthread_create(&recv_thread, NULL, message_dequeue_recv, self);
    if (err); //error
    self_listen(self);
    err = pthread_join(recv_thread, NULL);
    if (err); //error
}

// load_config sets self->config values to those in the config file, or the
// defaults if absent
void load_config(struct self *self){
    //buffer file = read_file("config");
    self->config.file_folder = "";
    self->config.udp_port = 1024;
}

// self_listen opens the configured socket and begins a listener thread to
// service requests from peers
void self_listen(struct self *self){
    //TODO: open a separate UDP socket for exclusive use with bulk file
    //  tranfers (the result of a successful rq_find_file request)
    //  this is to ensure that bulk transfers don't crowd out incoming messages
    //  if it fills the socket buffer
    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd == -1); //error
    in_port_t port = htons(self->config.udp_port);
    struct sockaddr_in6 addr = { AF_INET6, port, 0, IN6ADDR_ANY_INIT, 0 };
    socklen_t addrlen = sizeof(addr);
    if (bind(sockfd, (struct sockaddr *) &addr, addrlen)); //error
    //TODO: attempt bind with port 0 if denied due to in-use port
    //TODO: find optimal buffer size
    byte *buf = malloc(MAX_UDP_PAYLOAD);
    if (!buf); //error
    while (1){
        int length = MAX_UDP_PAYLOAD;
        struct sockaddr_storage sa;
        socklen_t salen = sizeof(addr);
        int ret = recvfrom(sockfd, buf, length, MSG_TRUNC,
                (struct sockaddr *) &sa, &salen);
        if (ret == -1); //error
        if (length < ret); //error
        length = ret;
        struct message *m = message_parse(self, buf, length);
        util_get_address(&m->address, (struct sockaddr *) &sa);
        message_enqueue_recv(self, m);
        // m is now owned by the recv thread
    }
    if (close(sockfd)); //error
    free(buf);
}

