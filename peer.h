#ifndef PEER_H
#define PEER_H

/// extern function declarations
struct peers *peer_create_list();
void peer_read_table(struct peers *);
void peer_listen(struct peers *);

#endif
