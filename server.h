#ifndef SERVER_H
#define SERVER_H

#include "libdsp.h"

/* initialize forks a new server instance and binds it to a dsp_self object.
 *  returns
 *      pointer to new dsp_self object
 */
struct dsp_self *initialize ();

#endif
