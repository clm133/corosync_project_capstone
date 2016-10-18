/* cluster_manager is responsible for add/remove node functionality,
*  checking/printing node health, checking/printing cluster membership, 
*  checking/printing cluster health
*/

#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <corosync/cfg.h>
#include <corosync/totem/totem.h>
#include <corosync/cmap.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>

int cmap_init(cmap_handle_t handle);
void cmap_close(cmap_handle_t handle);
int cfg_init(corosync_cfg_handle_t handle);
void cfg_close(corosync_cfg_handle_t handle);

int add_node(char *addr);
int print_ring();

#endif /* CLUSTER_MANAGER_H */