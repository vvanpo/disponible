#ifndef LIBDSP_H
#define LIBDSP_H

// libdsp can create or connect to a dsp node.

#define DSP_HASH_LENGTH 32

// Error codes
enum {
    DSP_E_INVALID,
    DSP_E_SYSTEM,
    DSP_E_DATABASE,
    DSP_E_NETWORK
};

typedef struct dsp_error * dsp_error;

// dsp_error_code returns the error code corresponding to the passed dsp_error
//  object.  Returns DSP_E_INVALID when passed an invalid error object.
int dsp_error_code (
    dsp_error
);

// dsp_error_message returns a message describing the passed error.  The
//  returned string is part of the dsp_error object and is freed on a call
//  to dsp_error_free().
char *dsp_error_message (
    dsp_error           // the error object to describe
);

// dsp_error_free frees the memory allocated to the dsp_error object.
void dsp_error_free (
    dsp_error           // the error object to be freed
);

// Instance object 
struct dsp;

// Peer identifier
struct dsp_node;

// dsp_init initializes an instance of a dsp node at <directory>, creating a new
//  node if one does not exist.
dsp_error dsp_init (
    char const *path,   // the relative or absolute path where the node instance
                        //  lives
    struct dsp **dsp    // OUT: new instance object
);

// dsp_bind establishes an authorized connection to a running instance.
dsp_error dsp_bind (
    char *address,      // the internet socket address to connect to
    struct dsp **dsp    // OUT: new instance object
);

// dsp_close disconnects from the running instance.
dsp_error dsp_close (
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
