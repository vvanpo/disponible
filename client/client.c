#include <stdio.h>

#include "client.h"

int main (int argc, char *argv[])
{
    struct dsp *dsp;
    dsp_error err;
    if (err = dsp_init("/tmp", &dsp))
        return -1;
    return 0;
}
