#ifndef NODE_H
#define NODE_H

#include "conf.h"

struct node {
	struct conf conf;
	void *key_pair;
	struct bucket *root;
	struct peer **add_queue;
};

int node_start(char *path);

#endif
