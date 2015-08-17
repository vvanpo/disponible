#include <string.h>

/// implementing header
#include "conf.h"

struct conf default_conf = {
	false,
	(char *[]) { "192.168.0.1:1024:0:1024", NULL },
	NULL,
	20,
	4,
	1024,
	1024,
	1024,
	(char *[]) {},
	(char *[]) {},
};

inline void conf_load_default(struct conf *conf)
{
	memcpy(conf, &default_conf, sizeof(*conf));
}

int conf_load_file(struct conf *conf, int fd)
{
	return 0;
}

int conf_get_option(char **value, enum conf_opt option, struct conf *conf)
{
	return 0;
}

int conf_set_option(enum conf_opt option, char *value, struct conf *conf)
{
	return 0;
}
