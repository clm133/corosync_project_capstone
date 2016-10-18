#include "cluster_manager.h"
#include "client_errors.h"

int cmap_init(cmap_handle_t handle)
{
	int err;
	err = cmap_initialize(&handle);
	return err;
}

void cmap_close(cmap_handle_t handle)
{
	(void)cmap_finalize(handle);
}

int cfg_init(corosync_cfg_handle_t handle)
{
	int err;
	err = corosync_cfg_initialize(&handle, NULL);
	return err;
}

void cfg_close(corosync_cfg_handle_t handle)
{
	(void)corosync_cfg_finalize(handle);
}

int add_node(char *addr)
{
	return 0;
}

int print_membership()
{
	cs_error_t err;
	corosync_cfg_handle_t handle;
	char **names; //array of node names
	char **status; //array of node status
	unsigned int count; //count of member nodes
	unsigned int i;
	unsigned int j;
	unsigned int node_id;
	
	printf("RING STATUS:");
	err = cfg_init(handle);
	//unsuccessful init
	if(err != CS_OK){
		printf("Failed to initialize corosync configuration API. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//upon successful init, try to get local node id
	err = corosync_cfg_local_get(handle, &node_id);
	//if local node id unsuccessful
	if(err != CS_OK){
		printf ("Failed to find local node id. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	else{
		printf("Local node ID: %u\n", node_id);
	}
	//Try to retrieve ring status
	err = corosync_cfg_ring_status_get(handle, &names, &status, &count);
	//if ring status unsuccessful
	if(err != CS_OK){
		printf ("Failed to get ring status. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//on success, loop through our name/status arrays
	else{
		for(i=0;i<count;i++){
			printf("Ring ID: %d\n", i);
			printf("\tid\t= %s\n", names[i]);
			printf ("\tstatus\t= %s\n", status[i]);
		}
		//free members of our arrays
		for(i=0;i<count;i++){
			free(names[i]);
			free(status[i]);
		}
		//free the arrays themselves
		free(names);
		free(status);
	}
	//close the handle
	cfg_close(handle);
	return err;
}