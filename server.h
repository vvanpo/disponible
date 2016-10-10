#ifndef SERVER_H
#define SERVER_H

#include "libdsp.h"

/* initialize forks a new server instance and binds it to a dsp_host object.
 *  returns
 *      pointer to new dsp_host object, NULL on error
 */
struct dsp_host *initialize ();

#endif
