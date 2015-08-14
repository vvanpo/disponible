#ifndef CONF_H
#define CONF_H

#include <stdbool.h>
#include <stdint.h>

struct conf {
	bool is_ephemeral;
	char **bootstrap;
	void *keys;
	int bucket_size;
	int bucket_depth_max;
	uint16_t port_sctp, port_tcp, port_udp;
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

int conf_load_file(struct conf *conf, int fd);
int conf_get_option(char **value, enum conf_opt option, struct conf *conf);
int conf_set_option(enum conf_opt option, char *value, struct conf *conf);
int conf_get_authorized(char ***keys, struct conf *conf);
int conf_add_authorized(char *key, struct conf *conf);
int conf_rm_authorized(char *key, struct conf *conf);
int conf_get_peer_blacklist(char ***finger, struct conf *conf);
int conf_add_peer_blacklist(char *finger, struct conf *conf);
int conf_rm_peer_blacklist(char *finger, struct conf *conf);

#endif
