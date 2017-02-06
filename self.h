#ifndef SELF_H
#define SELF_H

#include <stddef.h>
#include <stdbool.h>

#define HASH_LENGTH 32

// self.c
struct self;
struct self *new (char *directory, struct config *config);
struct self *load (char *directory);
struct self *bind (char *address);
void stop (struct self *self);

// config.c
struct config;
struct config *new_config ();
struct config *load_config (FILE *file);
void write_config (FILE *file);

// node.c
struct nodes;
struct nodes *new_nodes ();
struct nodes *load_nodes (char *directory);
void write_nodes (char *directory);

// crypto.c
struct keys;
struct public_key;
struct keys *new_keys ();
struct keys *load_keys ();
void hash (unsigned char *out, void const *in, size_t length);

// message.c
struct node *find_node (unsigned char *fingerprint);
void message_send (unsigned char *message, struct node *node);
void message_handler (void *message);

// socket.c
void listen (struct self *self);

#endif
