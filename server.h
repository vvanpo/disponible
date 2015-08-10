#ifndef DSP_SERVER_H
#define DSP_SERVER_H

#include "dsp_error.h"

enum dsp_srv_config {
    DSP_SRV_DIR, DSP_SRV_BOOTSTRAP_FILE, DSP_SRV_KEYS_FILE,
    DSP_SRV_BUCKET_SIZE, DSP_SRV_BUCKET_DEPTH_MAX,
    DSP_SRV_PORT_SCTP, DSP_SRV_PORT_TCP, DSP_SRV_PORT_UDP,
};

int dsp_srv_setup(char *path);
int dsp_srv_get_config(char **value, enum dsp_srv_config option);
int dsp_srv_set_config(enum dsp_srv_config option, char *value);
int dsp_srv_bootstrap();

#endif
