#ifndef SSH_MANAGER_H_DEFINED
#define SSH_MANAGER_H_DEFINED
#define LIBSSH_STATIC 1
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int start_corosync(char *addr);
int stop_corosync(char *addr);
int copy_conf(char *addr, char *source_file);

#endif /* SSH_MANAGER_H */