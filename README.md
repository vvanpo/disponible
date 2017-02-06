# dsp

**dsp** is a peer–peer communication protocol.

Every node is given a fingerprint based on a unique public key.  Every node
keeps a finite local list of valid nodes, identified by their fingerprint.  The
list is arranged as a series of buckets, the number of bits in the fingerprint
determining the number of buckets (i.e. 256 buckets for the SHA256 hash).  Each
bucket is of equal, finite (configurable) length, and valid node-fingerprints
are queued into buckets in an LRU fashion (the least recently-contacted node
being removed from a full bucket when a new node is found).  The buckets are
numbered such that a given node matching the first `n` bits of its fingerprint
with that of the host is placed into bucket (`n - # of buckets`).  Hence the
arithmetic difference between fingerprints is considered to be the "distance"
between nodes, and the host preferentially remembers valid nodes close by.  It
should be noted that this means the first bucket is always empty (as it equals
the host's fingerprint).

### Security

Nodes are verified by requesting their (signed) public key, and comparing its
hash to the given fingerprint.  To find a particular node not yet known to the
host, a request is sent to the known nodes closest to the node to be found. 
These requests traverse through the network until the node is found.

## Structure

A given instance has the following file structure:

    ./
        keys/
            public
            private
        nodes/
        authorized
        blacklist
        config

The `keys` folder contains the instance's base64-encoded public and private
keys.

The `nodes` folder contains a file for every node in the instance's node list, 
identified by their fingerprint (in hexadecimal).

`authorized` contains a list of base64-encoded public keys that grant priveleges
to send commands to the instance.

`blacklist` contains a list of fingerprints and network addresses that this
instance should ignore.

## Manual

**NAME**

    dsp

**SYNOPSIS**

    dsp [option [...]]

**DESCRIPTION**

    dsp instantiates a dsp node, or retrieves data and sends commands if an
    instance already exists (using the DSP_ADDRESS environment variable).

**OPTIONS**

    -h, --help
        Display this help and exit.

    -s, --start [<directory>]
        Start an existing dsp node at <directory>, present directory if empty.

    -i, --initialize [<directory>]
        Initialize a new dsp node, returning the fingerprint.

    -c, --config [option [...]]
        Reads config from stdin, although specified options take precedence.

        bootstrap=<fingerprint>
            A bootstrap node provides routes to peers to populate an empty nodes
            list.
