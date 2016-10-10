
// A client will allow a user to add or remove files from the file list, or query the file list for data about itself.
// All requests from the client happen through the open ports on the daemon, meaning requests are not restricted to coming from the local machine.
// For authentication, all client requests are encrypted with a public key in the server's authorized_keys file.  The server's own key is authorized by default.
