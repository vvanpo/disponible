#ifndef DSP_CLIENT_H
#define DSP_CLIENT_H

enum dsp_error {
	DSP_ERR_NO_KEY,
	DSP_ERR_INVALID_KEY,
	DSP_ERR_INVALID_HOST,
	DSP_ERR_UNREACHABLE,
	DSP_ERR_CONN_FAILURE,
	DSP_ERR_NODE_FAILURE,
	DSP_ERR_UNAUTHORIZED,
	DSP_ERR_INVALID_REQUEST,
};

struct dsp_conn;

int dsp_node_connect(struct dsp_conn **conn, char *key_path);
int dsp_get_file(char *hash, struct dsp_conn *conn);
int dsp_put_file(unsigned char *data, struct dsp_conn *conn);

#endif
