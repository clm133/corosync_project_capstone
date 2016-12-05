#ifndef CLIENT_CMAPCTL_H
#define CLIENT_CMAPCTL_H

#include <ctype.h>
#include <stdio.h>
#include <poll.h>
#include <corosync/corotypes.h>
#include <corosync/cmap.h>
#include <corosync/cfg.h>
#include <corosync/totem/totem.h>

#include "client_errors.h"

/* These are what corosync uses to enumerate cmap value types 
/* They can be found in cmap.h, but I put them here in comments
/* for reference because they are parameters for set_key_value() 
/* a and member in the cmap_value struct.

typedef enum {
    CMAP_VALUETYPE_INT8 	=  1,
    CMAP_VALUETYPE_UINT8	=  2,
    CMAP_VALUETYPE_INT16	=  3,
    CMAP_VALUETYPE_UINT16	=  4,
    CMAP_VALUETYPE_INT32	=  5,
    CMAP_VALUETYPE_UINT32	=  6,
    CMAP_VALUETYPE_INT64	=  7,
    CMAP_VALUETYPE_UINT64	=  8,
    CMAP_VALUETYPE_FLOAT	=  9,
    CMAP_VALUETYPE_DOUBLE	= 10,
    CMAP_VALUETYPE_STRING	= 11
} cmap_value_types_t; 				*/

typedef struct Cluster_Member {
	int nodeid;
	char ip[INET6_ADDRSTRLEN]; //at most will be the length of an ipv6 addr
	char status[32]; //should not be more than 32 characters
} Cluster_Member ;

/*type paremeter MUST have a correct type. see the enum above*/
int set_cmap_value(const char *key_name, void *value, cmap_value_types_t type);

/*value can be passed NULL, in which case, this function will return with a cmap_value_types_t type parameter set to the key's type */
int get_cmap_value(const char *key_name, void *value, cmap_value_types_t *type);

/*if this function is passed a value parameter, it will be set to the value being deleted. 
/*Otherwise, this can just be passed NULL, in which case, this function will return with a cmap_value_types_t type parameter set to the key's type */
int delete_cmap_value(const char *key_name, void *value, cmap_value_types_t *type);

int get_member_count(int *count);

int get_members(Cluster_Member **member_array, int array_len);

/* generates a key with prefix "nodelist.node.X."*/
int generate_nodelist_key(char *key_buffer, uint32_t id, char *key_suffix);

/*  generates a key with prefix "runtime.totem.pg.mrp.srp.members."*/
int generate_members_key(char *key_buffer, uint32_t id, char *key_suffix);

#endif /* CMAPCTL_H */