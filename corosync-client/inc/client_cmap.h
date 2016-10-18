#ifndef CLIENT_CMAP_H
#define CLIENT_CMAP_H
#include <stdio.h>
#include <stdlib.h>
#include <corosync/cmap.h>

int cmap_init(cmap_handle_t conn);
int cmap_close(cmap_handle_t conn);
int add_node(char *addr);

#endif /* CLIENT_CMAP_H */