#include <stdlib.h>

/// implementing header
#include "conf.h"

struct conf default_conf = {
	false,
	(char *[]) { "localhost:1024:1024", },
	NULL,
	20,
	4,
	1024,
	1024,
	1024,
};

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
