
/// implementing header
#include "prot.h"

static int key_agreement(struct peer *peer);

int prot_send(struct message *msg, struct peer *peer, struct node *node)
{
	return 0;
}

//
// Handshake:
//   Sender: Header + public_key + 
//

int prot_send_handshake(struct peer *peer)
{
	int err = key_agreement(peer);
	return 0;
}

int key_agreement(struct peer *peer)
{
	return 0;
}
