#ifndef DSP_NODE_H
#define DSP_NODE_H

#include "error.h"

enum dsp_config {
    DSP_SRV_BOOTSTRAP_FILE,
    DSP_SRV_KEYS_FILE,
    DSP_SRV_AUTHORIZED_KEYS,
    DSP_SRV_BUCKET_SIZE,
    DSP_SRV_BUCKET_DEPTH_MAX,
    DSP_SRV_PORT_SCTP,
    DSP_SRV_PORT_TCP,
    DSP_SRV_PORT_UDP,
    DSP_SRV_FILE_DIR,
    DSP_SRV_REMOTE_FILES,
    DSP_SRV_FILE_BLACKLIST,
};

struct dsp_node;

int dsp_setup(char *path);
// *value is allocated and must be freed after use
int dsp_get_config(char **value, enum dsp_config option, char *path);
int dsp_set_config(enum dsp_config option, char *command, char *path);
int dsp_bootstrap(char *path);

int dsp_start(struct **dsp_node node, char *path);
int dsp_stop(struct *dsp_node node);

int dsp_get_file(struct *dsp_node node);

#endif
