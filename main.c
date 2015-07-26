
#include "disponible.h"

int main(int argc, char **argv){
    struct di_options opts = { "" };
    struct di_daemon *d;
    di_daemon_load(&d, &opts);
    di_daemon_run(d);
    return 0;
}
