# dsp

**dsp** is a peer--peer communication protocol.

Every node is given a fingerprint based on a unique public key.  Every node keeps a local lists of known nodes, identified by their fingerprint.  The lists are arranged at the leaves of a binary tree, where every node within list n/k (where k is a configurable determining the number of lists) shares the first n bits of its fingerprint with the host.  When all lists are the same length, this results in more nodes that have a fingerprint close in value to that of the host.  Hence the arithmetic difference between hashes is considered to be the "distance" between nodes.

Nodes are verified by requesting their public key, and comparing its hash to the given fingerprint.  To find a particular node not yet known to the host, a request is sent to the known nodes closest to the node to be found.  These requests traverse through the network until the node is found.
