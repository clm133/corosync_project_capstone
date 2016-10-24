/* cluster_manager is responsible for add/remove node functionality,
*  checking/printing node health, checking/printing cluster membership, 
*  checking/printing cluster health
*/

#ifndef CLUSTER_MANAGER_H_DEFINED
#define CLUSTER_MANAGER_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <corosync/cfg.h>
#include <corosync/totem/totem.h>
#include <corosync/cmap.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>

int add_node(char *addr);
int print_ring();
int print_members();

#endif /* CLUSTER_MANAGER_H */