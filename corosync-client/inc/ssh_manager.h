#ifndef SSH_MANAGER_H_DEFINED
#define SSH_MANAGER_H_DEFINED
#define LIBSSH_STATIC 1

#include <libssh.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "client_errors.h"

//helper function which frees channel for ssh functions
void free_channel(ssh_channel channel);
//helper function which frees session for ssh functions
void free_session(ssh_session session);
//sends the command 'corosync' to the address provided, returns 0 on success or client_error_t on 
int start_corosync(char *addr);
//sends the command 'corosync-cfgtool -H' to the address provided, returns 0 on success or client_error_t on 
int stop_corosync(char *addr);
//just calls start_corosync and then stop_corosync()
int restart_corosync(char *addr);

#endif /* SSH_MANAGER_H */