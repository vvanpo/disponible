#ifndef _LIBDSP_H
#define _LIBDSP_H

#define DSP_HASH_LENGTH 32

// server identifier
struct dsp_host;
struct dsp_node {
    unsigned char fingerprint[DSP_HASH_LENGTH];
};

/* dsp_connect binds a dsp_host object to a host.
 *  returns
 *      pointer to dsp_host object
 */
struct dsp_host *dsp_connect ();

/* dsp_list_stored_nodes traverses the node list and returns a list of
 *      fingerprints
 *  accepts
 *      pointer to existing int
 *  returns
 *      newly allocated contiguous block of length (*num * DSP_HASH_LENGTH),
 *          initialized with the ordered list of fingerprints
 *      updates *num to number of returned nodes
 */
unsigned char *dsp_list_stored_nodes (int *num, struct dsp_host *);

/* dsp_return_node returns the node with the given fingerprint.  If the node is
 *      not stored, the network is queried to find the node.
 *  accepts
 *      fingerprint of length DSP_HASH_LENGTH
 *  returns
 *      new dsp_node object, zero-filled if node not found
 */
struct dsp_node dsp_return_node (unsigned char *fingerprint, struct dsp_host *);

#endif
