#ifndef CONF_H
#define CONF_H

#include <stdbool.h>
#include <stdint.h>
#include "peer.h"

struct conf {
	bool is_ephemeral;
	struct peer **bootstrap;
	int bucket_size;
	int bucket_depth_max;
	uint16_t port_sctp, port_tcp, port_udp;
	char **authorized;
	struct peer *blacklist;
};

enum conf_opt {
	IS_EPHEMERAL,
	BOOTSTRAP_FILE,
	KEYS_FILE,
	BUCKET_SIZE,
	BUCKET_DEPTH_MAX,
	PORT_SCTP,
	PORT_TCP,
	PORT_UDP,
};

void conf_load_default(struct conf *conf);
int conf_load_file(struct conf *conf, int fd);
int conf_get_authorized(char ***keys, struct conf *conf);
int conf_add_authorized(char *key, struct conf *conf);
int conf_rm_authorized(char *key, struct conf *conf);
int conf_get_peer_blacklist(char ***finger, struct conf *conf);
int conf_add_peer_blacklist(char *finger, struct conf *conf);
int conf_rm_peer_blacklist(char *finger, struct conf *conf);

#endif
