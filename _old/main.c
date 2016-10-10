
#include <stdio.h>
#include <unistd.h>
#include "disponible.h"

int main(int argc, char **argv){
    if (argc > 1)
        if (chdir(argv[0])){
            fprintf(stderr, "Could not access %s\n", argv[0]);
            return -1;
        }
    struct di_options opts = { "" };
    struct di_daemon *d;
    di_daemon_load(&d, &opts);
    di_daemon_run(d);
    return 0;
}
