#ifndef NODE_H
#define NODE_H

enum error {
	ERR_PATH_NOT_EMPTY,
	ERR_NO_PERMISSION,
	ERR_INVALID_CONFIG,
	ERR_INVALID_FORMAT,
	ERR_CONNECT,
	ERR_NO_KEY,
};

enum config {
	BOOTSTRAP_FILE,
	KEYS_FILE,
	AUTHORIZED_KEYS,
	BUCKET_SIZE,
	BUCKET_DEPTH_MAX,
	PORT_SCTP,
	PORT_TCP,
	PORT_UDP,
	FILE_DIR,
	REMOTE_FILES,
	FILE_BLACKLIST,
};

int setup(char *path);
// *value is allocated and must be freed after use
int get_config(char **value, enum config option, char *path);
int set_config(enum config option, char *command, char *path);
int bootstrap(char *path);

int start(char *path);

#endif
