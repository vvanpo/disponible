#ifndef PEER_H
#define PEER_H

#include "cryp.h"
#include "net.h"

struct peer {
	unsigned char finger[HASH_LEN];
	struct address addr;
	void *public_key;
	struct session *session;
};
struct node;

void peer_get(struct peer **peer, unsigned char *finger, struct node *node);
int peer_add(unsigned char *finger, struct node *node);
void peer_remove(struct peer *peer, struct node *node);

#endif
