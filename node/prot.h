#ifndef PROT_H
#define PROT_H

#include "peer.h"

struct message;

int prot_send(struct message *msg, struct peer *peer, struct node *node);
//int prot_recv(char const *buf, struct node *node);

int prot_send_handshake(struct peer *peer);

#endif
