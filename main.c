
#include "self.h"

int main(int argc, char **argv){
    struct self *self = self_load_config();
    peer_read_table(self->peers);
    return 0;
}
