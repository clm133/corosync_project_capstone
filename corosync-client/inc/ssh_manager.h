#ifndef SSH_MANAGER_H_DEFINED
#define SSH_MANAGER_H_DEFINED
#define LIBSSH_STATIC 1

#include <libssh.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "client_errors.h"
#include "time_manager.h"

//helper function which frees channel for ssh functions
void free_channel(ssh_channel channel);
//helper function which frees session for ssh functions
void free_session(ssh_session session);
//sends the command 'corosync' to the address provided, returns 0 on success or client_error_t on failure
int start_corosync(char *addr);
//sends the command 'corosync-cfgtool -H' to the address provided, returns 0 on success or client_error_t on failrue
int stop_corosync(char *addr);
//shuts down eth1 at addr (must use 12.0.0.X addr)
int kill_conn(char *addr);
//brings up eth1 at addr (must use 12.0.0.X addr)
int start_conn(char *addr);
// just a call to stop_corosync() and then start_corosync()
int restart_corosync(char *addr);

#endif /* SSH_MANAGER_H */