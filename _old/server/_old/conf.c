#include <stdlib.h>
#include <string.h>
#include "cryp.h"
#include "net.h"

/// implementing header
#include "conf.h"

inline void conf_load_default(struct conf *conf)
{
	struct conf default_conf = {
		.is_ephemeral = false,
		.peer_add_queue_len = 10,
		.bucket_size = 20,
		.bucket_depth_max = 4,
		.port_sctp = 1024,
		.port_udp = 1024,
	};
	memcpy(conf, &default_conf, sizeof *conf);
	default_conf.bootstrap = calloc(2, sizeof *default_conf.bootstrap);
	default_conf.bootstrap[0] = calloc(1, sizeof **default_conf.bootstrap);
	memcpy(default_conf.bootstrap[0]->finger,
		"abcdefghijklmnopqrstuvwxyz012345", HASH_LEN);
	net_parse_addr(&default_conf.bootstrap[0]->addr, "[::1]:1024:0:1024");
}

int conf_load_file(struct conf *conf, int fd)
{
	return 0;
}

