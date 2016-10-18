#include "client_cmap.h"

int cmap_init(cmap_handle_t conn)
{
	int err;
	err = cmap_initialize(&conn);
	printf("cmap initialized...\n");
	return err;
}

int cmap_close(cmap_handle_t conn)
{
	int err;
	cmap_finalize(conn);
	printf("cmap closed...\n");
	return 0;
}

int add_node(char *addr)
{
	int err;
	cmap_handle_t conn; //our connection
	
	err = cmap_init(conn);
	printf("%s added to cluster.\n", addr);
	err = cmap_close(conn);
	return err;
}