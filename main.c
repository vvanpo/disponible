
#include "self.h"

int main(int argc, char **argv){
    struct self *self = self_load();
    self_run_daemon(self);
    return 0;
}
