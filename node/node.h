#ifndef NODE_H
#define NODE_H

#include "conf.h"

enum error {
	ERR_SUCCESS = 0,
	ERR_SYSTEM,
	ERR_PATH_INVALID,
	ERR_PATH_NOT_EMPTY,
	ERR_NO_PERMISSION,
	ERR_FORMAT_INVALID,
	ERR_CONN_FAILURE,
	ERR_NO_KEY,
};

struct node {
	struct conf *conf;
	void *key_pair;
	struct bucket *root;
};

int node_start(char *path);

#endif
