#ifndef _LIBDSP_H
#define _LIBDSP_H

#define DSP_HASH_LENGTH 32

// instance object 
struct dsp_self;
// peer identifier
struct dsp_node {
    unsigned char fingerprint[DSP_HASH_LENGTH];
};

/* dsp_connect establishes an authorized connection to a node, to send commands
 *      and retrieve data.
 *  returns
 *      pointer to dsp_self object
 */
struct dsp_self *dsp_connect ();

/* dsp_list_nodes traverses the node list and returns a list of fingerprints
 *  accepts
 *      pointer to existing int
 *  returns
 *      newly allocated array of length (*num * DSP_HASH_LENGTH), initialized
 *          with the ordered list of fingerprints
 *      updates *n to number of returned nodes
 */
unsigned char *dsp_list_nodes (int *n, struct dsp_self *);

/* dsp_return_node returns the dsp_node object corresponding to the given
 *      fingerprint.  If the node is not stored, the network is queried to find
 *      the node.
 *  accepts
 *      fingerprint of length DSP_HASH_LENGTH
 *  returns
 *      new dsp_node object, zero-filled if node not found
 */
struct dsp_node dsp_return_node (unsigned char *fingerprint, struct dsp_self *);

#endif
