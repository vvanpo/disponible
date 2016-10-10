#ifndef _LIBDSP_H
#define _LIBDSP_H

#define DSP_HASH_LEN 32

struct dsp_self;
struct dsp_node {
    unsigned char fingerprint[DSP_HASH_LEN];
};
struct dsp_file {
    unsigned char *name;
    unsigned char hash[DSP_HASH_LEN];
    // File size in bytes
    unsigned int size;
    // Block size (in bytes) is 2^block_len_exp
    unsigned char block_sz_exp;
    // Array of stored block numbers, null if the whole file is stored
    unsigned int *blocks;
};

/* dsp_connect binds a dsp_self object to a remote or local host.
 *  returns
 *      pointer to dsp_self object
 */
struct dsp_self *dsp_connect ();

/* dsp_list_stored_nodes traverses the node list and returns a list of
 *      fingerprints
 *  accepts
 *      pointer to existing int
 *  returns
 *      newly allocated contiguous block of length (*num * DSP_HASH_LEN),
 *          initialized with the ordered list of fingerprints
 *      updates *num to number of returned nodes
 */
unsigned char *dsp_list_stored_nodes (int *num, struct dsp_self *);

/* dsp_list_stored_files returns a list of stored files
 *  accepts
 *      pointer to existing int
 *  returns
 *      newly allocated array of file structs
 *      updates *num to number of files
 */
struct file *dsp_list_stored_files (int *num, struct dsp_self *);

/* dsp_return_node returns the node with the given fingerprint.  If the node is
 *      not stored, the network is queried to find the node.
 *  accepts
 *      fingerprint of length DSP_HASH_LEN
 *  returns
 *      new dsp_node object, zero-filled if node not found
 */
struct dsp_node dsp_return_node (unsigned char *fingerprint, struct dsp_self *);

#endif
