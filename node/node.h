#ifndef NODE_H
#define NODE_H

struct node {
	struct conf *conf;
	void *key_pair;
	struct bucket *root;
};

int node_start(char *path);

#endif
