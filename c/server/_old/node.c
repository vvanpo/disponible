#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "cryp.h"
#include "error.h"
#include "net.h"
#include "prot.h"

/// implementing header
#include "node.h"

static int init_instance(struct node *node);
static int bootstrap(struct node *node);
static int listen(struct node *node);

// node_start starts the daemon, using the passed directory for loading config
//   and persistence if it is non-null.  If a null-pointer is passed then the
//   resulting instance is in-memory only.
int node_start(char *path)
{
	struct node *node = calloc(1, sizeof *node);
	if (!node) return ERR_SYSTEM;
	if (!path) {
		conf_load_default(&node->conf);
		node->conf.is_ephemeral = true;
	} else {
	//	conf_load_file
	//	init_instance
	}
	int err;
	if (err = cryp_gen_keypair(&node->key_pair)) return err;
	//
	// always call bootstrap for now
	//
	if (err = bootstrap(node)) return err;
	if (err = listen(node)) return err;
	free(node);
	return 0;
}

// init_instance initializes the persistent instance storage by creating the
//   directories
// 	peers/
// 	files/queue/
// 	keys/
//   On error, returns:
//	ERR_NO_PERMISSION
//	ERR_PATH_NOT_EMPTY
//	ERR_SYSTEM
int init_instance(struct node *node)
{
	/*
	char *dirs[] = { "peers", "files", "files/queue", "keys" };
	for (int i = 0; i < 4; i++) {
		int mode = strcmp(dirs[i], "keys") ? 755 : 700;
		if (mkdirat(fd, dirs[i], mode)) {
			switch (errno) {
			case EACCES:
			case EROFS:
			case EPERM:
				return ERR_NO_PERMISSION;
			case EEXIST:
				return ERR_PATH_NOT_EMPTY;
			case EMLINK:
			case ENOSPC:
			case ENOMEM:
				return ERR_SYSTEM;
			default:
				assert(false);
			}
		}
	}
	*/
	// crypt_create_keys()
	return 0;
}

int bootstrap(struct node *node)
{
	struct peer *p;
	for (int i = 0; (p = node->conf.bootstrap[i]); i++) {
		int err = net_connect(p);
		//TODO: if net error... continue;
		err = prot_send(NULL, p, node);
	}
	return 0;
}

int listen(struct node *node)
{
	return 0;
}
