
#include "self.h"

int main(int argc, char **argv){
    struct self *self = self_load();
    self_start_daemon(self);
    return 0;
}
