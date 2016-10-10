# dsp

**dsp** is a peer--peer communication protocol.

Every node is given a fingerprint based on a unique public key.  Every node keeps a finite local list of valid nodes, identified by their fingerprint.  The list is arranged as a series of buckets, the number of bits in the fingerprint hash determining the number of buckets (i.e. 256 for the SHA256 hash).  Each bucket is of equal, finite length, and valid node fingerprints are queued into buckets in an LRU fashion (the last used node being removed from a full bucket when a new node is found).  The buckets are numbered such that a given node matching the first n bits of its fingerprint with that of the host is placed into bucket (n - # of buckets).  Hence the arithmetic difference between fingerprints is considered to be the "distance" between nodes, and the host preferentially remembers valid nodes close by.  It should be noted that this means the first bucket is always empty (as it equals the host's fingerprint).

Nodes are verified by requesting their public key, and comparing its hash to the given fingerprint.  To find a particular node not yet known to the host, a request is sent to the known nodes closest to the node to be found.  These requests traverse through the network until the node is found.

A given server has the following file structure:

nodes/
files/
keys/
    authorized
    private
    public
config

## Client

*NAME*
    dsp

*SYNOPSIS*
    *dsp* [option [...]]

*OPTIONS*
    *-h, --help*
        Display help and exit.
    *-s, --server* [<directory> | <fingerprint>] [option [...]]
        Binds the client to the server with fingerprint <fingerprint>, or living
        at <directory>.  If <directory> is specified but no server instance is
        running, the server is initialized.  Server options:
        *ephemeral*
            Initializes an ephemeral (in-memory) server hosting files at
            <directory>.  Must be used with *bootstrap=*, and no existing
            server can be specified.
        *bootstrap=*<fingerprint>
            Bootstrap node to populate empty stored nodes list.
        *node-bucket-size=*<exponent>
            Sets the number of allowable stored nodes by specifying bucket size
            as 2^<exponent>.  The number of buckets is equal to the number of
            bits in a fingerprint.  Must be a value between 0 and 255.

