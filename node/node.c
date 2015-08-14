#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>

/// implementing header
#include "node.h"

static int setup(struct node *node);
static int bootstrap(struct node *node);

int node_start()
{
	// always call bootstrap for now
	return 0;
}

/*
 * setup fills the passed directory with:
 * 	peers/
 * 	files/queue/
 * 	keys/
 *   returning error on failure:
 *	ERR_NO_PERMISSION
 * 	ERR_PATH_NOT_EMPTY
 *	ERR_SYSTEM
 */
int setup(struct node *node)
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
	//prot_send_auth(finger, addr);
	return 0;
}
