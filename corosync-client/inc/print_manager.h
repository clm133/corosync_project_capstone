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

int print_membership();
int print_quorum();

#endif /* PRINT_MANAGER_H */