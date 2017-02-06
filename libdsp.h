#ifndef _LIBDSP_H
#define _LIBDSP_H

/* libdsp can create or connect to a dsp node.
 */

// instance object 
struct dsp_self;
// configuration values
struct dsp_config;
// peer identifier
struct dsp_node;

/* dsp_new creates a new instance of a dsp node at <directory>,
 *  accepts
 *      directory string: the relative or absolute address where the node
 *          instance lives
 *      pointer to dsp_config object
 *  returns
 *      pointer to a dsp_self object
 */
struct dsp_self *dsp_new (char *directory, struct dsp_config *config);

/* dsp_load starts an existing instance of a dsp node living <directory>.
 *  accepts
 *      directory string: the relative or absolute address where the node
 *          instance lives
 *  returns
 *      pointer to a dsp_self object
 */
struct dsp_self *dsp_load (char *directory);

/* dsp_bind establishes an authorized connection to a running instance.
 *  accepts
 *      address string: the internet socket address to connect to
 *  returns
 *      pointer to a dsp_self object
 */
struct dsp_self *dsp_bind (char *address);

/* dsp_list_nodes traverses the nodes list and returns a list of fingerprints
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
 *      new dsp_node object, null if node not found
 */
struct dsp_node *dsp_return_node (unsigned char *fingerprint, struct dsp_self *);

#endif
