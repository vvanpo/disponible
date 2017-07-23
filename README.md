# dsp

**dsp** is a peer–peer communication protocol.

Every node is given a fingerprint based on a unique public key.  Fingerprints
are stored in a distributed hash table throughout the network.  Every node
indexes a list of valid nodes, identified by their fingerprint.  The list is
arranged as a series of buckets, the number of bits in the fingerprint
determining the number of buckets (i.e. 256 buckets for the SHA256 hash).  Each
bucket is of equal, finite (configurable) length, and valid fingerprints are
queued into buckets in an LRU fashion (the least recently-contacted node being
removed from a full bucket when a new node is found).  The buckets are numbered
such that a given node matching the first `n` bits of its fingerprint with that
of the host is placed into bucket (`n - # of buckets`).  Hence the arithmetic
difference between fingerprints is considered to be the "distance" between
nodes, and the host preferentially remembers valid nodes close by.

## Sessions

A session is initiated by sending a signed ephemeral public key.  The receiving
node creates a secret session key, encrypts it with the sender's ephemeral key,
and signs the message.  A session lasts for the duration of a single request.

## Node lookup

To find the node associated with a given fingerprint, the host queries a number
of nodes closest to said fingerprint.  If the query comes up empty, the search
is expanded by querying nodes further away, as well as requesting already-
queried nodes to perform their own queries.  This pattern continues until the
node is found, or all nodes have been queried repeatedly until they give up
(either by exhausting their own node list or reaching a preset limit on query-
depth).  A list of queried nodes is passed back-and-forth to ensure no node is
sent the same query more than once.

## Client manual

**NAME**

    dsp

**SYNOPSIS**

    dsp [OPTIONS]

    dsp COMMAND [ARG...]

**DESCRIPTION**

    dsp instantiates a dsp node, or retrieves data and sends commands if an
    instance already exists.

**OPTIONS**

    -h, --help
        Display this help and exit.

    -i, --interactive

**COMMANDS**
