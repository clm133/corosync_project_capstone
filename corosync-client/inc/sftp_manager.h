#ifndef SFTP_MANAGER_H_DEFINED
#define SFTP_MANAGER_H_DEFINED
#define LIBSSH_STATIC 1
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "client_errors.h"
#include "ssh_manager.h"

/* addr=the address of the node to sftp this file to, source_file = the file to copy, dest_directory = the place to copy it*/
int copy_conf(char *addr, char *source_file, char *dest_directory);
#endif /* SFTP_MANAGER_H */