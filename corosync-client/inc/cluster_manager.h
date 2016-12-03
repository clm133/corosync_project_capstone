#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H

#include <corosync/cmap.h>
#include <corosync/cfg.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>
#include "client_cmapctl.h"
#include "client_errors.h"

int get_local_nodeid(unsigned int *local_nodeid);

#endif /* CLUSTER_MANAGER_H */