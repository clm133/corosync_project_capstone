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

int print_membership()
{
	struct node_info{
		uint32_t node_id;
        char node_name[128];
    }nodes[4]; //only 4 for now. can change to support more in the future
	int err;
	cmap_handle_t conn; 
	cmap_iter_handle_t iter_h;
	uint32_t node_id ;
	
	//initialize connection and iterator
	err = cmap_init(conn);
	if(err != 0){
		printf("Could not establish connection\n");
		return err;
	}
	err = cmap_iter_init(conn, "nodelist.node", &iter_h);
	if(err != 0){
		printf("Could not establish cmap iterator\n");
		return err;
	}
	while(1){
		char key_name[32];
        size_t val_len = 0;
        cmap_value_types_t val_type;
        err = cmap_iter_next(conn, iter_h, key_name, &val_len, &val_type);
		if (err != 0) {
            break;
        }
		printf("This is a key_name: %s\n", key_name);
	}
	(void)cmap_iter_finalize (conn, iter_h);
	err = cmap_close(conn);
	return err;
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