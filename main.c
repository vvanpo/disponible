
#include "disp.h"
#include "self.h" //TODO: main should only include disp.h, which defines the API
                  //    intended for other applications to use.

int main(int argc, char **argv){
    struct self *self = self_load();
    self_run_daemon(self);
    return 0;
}
