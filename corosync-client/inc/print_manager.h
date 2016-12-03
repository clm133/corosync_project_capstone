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
#include "quorum_manager.h"
#include "cluster_manager.h"
#include "time_manager.h"
#include "monitor.h"

enum Cluster_Change{
	CLUSTER_JOINED,
	CLUSTER_LEFT
};

typedef struct Notify_Context {
	enum Cluster_Change change;
	char target_name[32]; //ip usually
	uint32_t target_nodeid;
	long context_start;
	long context_end;
} Notify_Context ;

int print_membership();
int print_quorum();
int print_notification(Notify_Context *nc);

#endif /* PRINT_MANAGER_H */