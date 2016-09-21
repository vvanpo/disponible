#ifndef _LIBDSP_H
#define _LIBDSP_H

#define DSP_HASH_LEN 32

struct dsp_self;
struct dsp_node {
    unsigned char[DSP_HASH_LEN] finger;
};

/* dsp_init binds a dsp_self object to the local host environment.
 *  returns
 *      pointer to new dsp_self object
 */
struct dsp_self *dsp_init ();

/* dsp_find_node finds the node with the given fingerprint.  If the node is not
 *      in the list of known nodes, the network is queried to find the node.
 *  accepts
 *      fingerprint of length DSP_HASH_LEN
 *      pointer to dsp_self object
 *  returns
 *      new dsp_node object, zero-filled if node not found
 */
struct dsp_node dsp_find_node (unsigned char *finger, struct dsp_self *);

/* dsp_list_valid_nodes traverses the node list and returns a list of
 *      fingerprints
 *  accepts
 *      pointer to existing int
 *      pointer to dsp_self object
 *  returns
 *      newly allocated contiguous block of length *len * DSP_HASH_LEN,
 *          initialized with the ordered list of fingerprints
 *      updates *len to number of fingerprints
 */
unsigned char *dsp_list_known_nodes (int *len, struct dsp_self *);

void dsp_fork_listener (struct dsp_self *);

#endif
