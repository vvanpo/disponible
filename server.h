#ifndef DSP_SERVER_H
#define DSP_SERVER_H

#include "error.h"

enum dsp_srv_config {
    DSP_SRV_BOOTSTRAP_FILE,
    DSP_SRV_KEYS_FILE,
    DSP_SRV_AUTHORIZED_KEYS,
    DSP_SRV_BUCKET_SIZE,
    DSP_SRV_BUCKET_DEPTH_MAX,
    DSP_SRV_PORT_SCTP,
    DSP_SRV_PORT_TCP,
    DSP_SRV_PORT_UDP,
    DSP_SRV_FILE_DIR,
};

int dsp_srv_setup(char *path);
// *value is allocated and must be freed after use
int dsp_srv_get_config(char **value, enum dsp_srv_config option, char *path);
int dsp_srv_set_config(enum dsp_srv_config option, char *command, char *path);
int dsp_srv_bootstrap(char *path);

#endif
