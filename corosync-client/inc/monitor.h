#ifndef MONITOR_H_DEFINED
#define MONITOR_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <limits.h>

#include <corosync/totem/totem.h>
#include <corosync/cfg.h>
#include <corosync/cmap.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>
#include "client_errors.h"
#include "time_manager.h"

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

int monitor_single_dispatch();
int monitor_status();

#endif