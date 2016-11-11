#ifndef CONF_MANAGER_H_DEFINED
#define CONF_MANAGER_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client_errors.h"
#include "client_cmapctl.h"
#include "cluster_manager.h"

/*copies a string representation of a conf file line in the buffer at the line parameter (this is largely a helper function for write_conf())*/
int create_conf_line(char *key_name, char *value_name, char *line);
/* writes the local corosync cmap values to file_name */
int write_conf(char *file_name);

#endif  /* CONF_MANAGER_H */