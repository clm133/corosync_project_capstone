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
#include "print_manager.h"

int monitor_status();

#endif