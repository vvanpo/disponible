#include "self.h"

void load_config (struct self *self)
{
    if (!self.path) self.path = "~/.dsp";
}
