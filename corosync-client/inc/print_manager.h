#ifndef PRINT_MANAGER_H_DEFINED
#define PRINT_MANAGER_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>
#include <string.h>
#include <time.h>

#include <corosync/totem/totem.h>
#include <corosync/cfg.h>
#include <corosync/cmap.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>
#include "client_errors.h"
#include "client_cmapctl.h"
#include "monitor.h"
#include "quorum_manager.h"
#include "cluster_manager.h"
#include "time_manager.h"

//prints membership (node id, ip, status and votes)
int print_membership();
//prints quorum (quorate, number of votes received, number of votes needed, also prints the time)
int print_quorum();

#endif /* PRINT_MANAGER_H */