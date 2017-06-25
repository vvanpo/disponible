#ifndef LIBDSP_H
#define LIBDSP_H

// libdsp can create or connect to a dsp node.

#define DSP_HASH_LENGTH 32

// Error codes
enum {
    DSP_OK,
    DSP_ESYSTEM,
    DSP_ESQLITE
};

// dsp_error returns a description corresponding to the passed error code.
//  Returned string does not need to be freed.
char *dsp_error (
    int code            // error code returned by dsp functions
);

// Instance object 
struct dsp;

// Peer identifier
struct dsp_node;

// dsp_init initializes an instance of a dsp node at <directory>, creating a new
//  node if one does not exist.
int dsp_init (
    char *path,         // the relative or absolute path where the node instance
                        //  lives
    struct dsp **dsp    // OUT: new instance object
);

// dsp_bind establishes an authorized connection to a running instance.
int dsp_bind (
    char *address,      // the internet socket address to connect to
    struct dsp **dsp    // OUT: new instance object
);

// dsp_close disconnects from the running instance.
int dsp_close (
    struct dsp *dsp
);
/*
// dsp_list_nodes traverses the nodes list and returns a list of fingerprints as
//  an array of length (*n * DSP_HASH_LENGTH), initialized with the ordered list
//  of fingerprints. *n is updated to the number of returned nodes.
int dsp_list_nodes (
    struct dsp *dsp,
    int *n,                         // pointer to existing int
    unsigned char **fingerprints    // OUT: 
);

// dsp_get_node returns a dsp_node object corresponding to the given
//  fingerprint.  If the node is not stored, the network is queried to find
//      the node.  Returns null if no node is found.
int dsp_get_node (
    struct dsp *dsp,
    unsigned char *fingerprint, // fingerprint of length DSP_HASH_LENGTH
    struct dsp_node **node      // OUT: 
);
*/
#endif
