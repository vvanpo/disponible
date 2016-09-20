# dsp

**dsp** is a peer--peer communication protocol.

Every node is given a fingerprint based on a unique public key.  Every node keeps a local list of known peers, identified by their fingerprint.  The peer list orders peers by their hash value, with the arithmetic difference between hashes considered to be the "distance" between peers.

Peers are verified by requesting their public key, and comparing its hash to the given fingerprint.
