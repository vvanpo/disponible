#include <string.h>
#include "cryp.h"

/// implementing header
#include "conf.h"

inline void conf_load_default(struct conf *conf)
{
	struct conf default_conf = {
		.is_ephemeral = false;
		.bucket_size = 20;
		.bucket_depth_max = 4;
		.port_sctp = .port_udp = 1024;
	};
	memcpy(conf, &default_conf, sizeof *conf);
	default_conf.bootstrap = calloc(2, sizeof (struct peer *));
	default_conf.bootstrap[0] = calloc(1, sizeof (struct peer));
	memcpy(default_conf.bootstrap[0].finger,
		"abcdefghijklmnopqrstuvwxyz012345", HASH_LEN);
}

int conf_load_file(struct conf *conf, int fd)
{
	return 0;
}

