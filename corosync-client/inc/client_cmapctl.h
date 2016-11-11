#ifndef CLIENT_CMAPCTL_H
#define CLIENT_CMAPCTL_H

#include <ctype.h>
#include <stdio.h>
#include <poll.h>
#include <corosync/corotypes.h>
#include <corosync/cmap.h>

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

int client_set_cmap_value(const char *key_name, void *value, cmap_value_types_t type);

int client_get_cmap_value(const char *key_name, void *value, cmap_value_types_t type);

int client_get_cmap_string_value(const char *key_name, char **value);

int client_delete_cmap_value(const char *key_name, void *value, cmap_value_types_t type);

int client_delete_cmap_string_value(const char *key_name, char **value);

int client_get_cmap_key_type(const char *key_name, cmap_value_types_t *type, size_t *len);

int nodelist_get_total(int *total);

int nodelist_get_id_array(unsigned int **list);

int nodelist_get_addr_array(char **list);

#endif /* CMAPCTL_H */