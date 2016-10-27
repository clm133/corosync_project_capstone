#include "cluster_manager.h"
#include "client_errors.h"
#include "modified_cmapctl.h"
#include "conf_writer.h"

cs_error_t get_highest_node(uint32_t *highest_id)
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	size_t value_len;
	cmap_value_types_t type;
	const char *generic_key = "nodelist.node.X.";
	uint32_t id;

	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//initialize iterator handle (we are looking for all nodeid)
	err = cmap_iter_init(cmap_handle, "nodelist.node.", &iter_handle);
	if(err != CS_OK){
		printf("Failed to initialize iterator. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//iterate through list
	*highest_id = 0;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		//find nodeid key
		if(strcmp(&key_name[strlen(generic_key)], "nodeid") == 0){
			//get nodeid
			err = cmap_get_uint32(cmap_handle, key_name, &id);
			//compare with current highest id
			if(id > *highest_id){
				*highest_id = id;
			}
		}
	}
	//close the handles
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);

	return CS_OK;
}


// add node

int add_node(char *addr)
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	uint32_t id;
	char set_id_key[124];
	char set_ring0_key[124];
	char id_char[32];
	char *nodelist = "nodelist.node.";
	char *set_id = ".nodeid";
	char *set_addr = ".ring0_addr";

	err = get_highest_node(&id);
	if(err != CS_OK){
		return -1;
	}
	//new node id will be one higher than previous
	id++;
	sprintf(id_char, "%u", (unsigned int)id);
	//key = nodelist.X.node.nodeid
	strcpy(set_id_key, nodelist);
	strcat(set_id_key, id_char);
	strcat(set_id_key, set_id);
	//key = nodelist.X.node.ring0_addr
	strcpy(set_ring0_key, nodelist);
	strcat(set_ring0_key, id_char);
	strcat(set_ring0_key, set_addr);
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return -1;
	}
	cmap_set_uint32(cmap_handle, set_id_key, id);
	cmap_set_string(cmap_handle, set_ring0_key, addr);
	(void)cmap_finalize(cmap_handle);
	//write to conf file
	err = write_config("corosync.conf");
	return err;
}



// remove node
// we should know the id to remove the node we want
// name the address "key" for convenience.
int remove_node(uint32_t id){

	cs_error_t err;
	cmap_handle_t cmap_handle;
	uint32_t i;
	//char *str;
	char id_key[124];
	char id_char[32];
	char *nodelist = "nodelist.node.";
	char *set_id = ".nodeid";
	//char *set_addr = ".ring0_addr";
	sprintf(id_char, "%u", (unsigned int)id);

	//key = nodelist.X.node.nodeid
	strcpy(id_key, nodelist);
	strcat(id_key, id_char);
	strcat(id_key, set_id);

	// initialize cmap_handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return -1;
	}

	// get key, catch error
	err = cmap_get_uint32(cmap_handle, id_key, &i);
	if(err != CS_OK){
		printf("Failed to retrieve key. Error#%d: %s\n", err, get_error(err));
		return -1;
	}
	// the key exists, delete that node
	err = cmap_delete(cmap_handle,id_key);
	if(err != CS_OK){
		printf("Failed to delete key. Error#%d: %s\n", err, get_error(err));
		return -1;
	}
	// finalize
	(void)cmap_finalize(cmap_handle);
	// write to conf file
	//err = write_config("corosync.conf");
	return err;
}







int print_ring()
{
	cs_error_t err;
	corosync_cfg_handle_t handle;
	char **names; //array of node names
	char **status; //array of node status
	unsigned int count; //count of member nodes
	unsigned int i;
	unsigned int node_id;

	printf("PRINTING RING STATUS:\n");
	err = corosync_cfg_initialize(&handle, NULL);
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
		printf("%s\t%s%u\n", "Local_node", "ID: ", node_id);
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
			printf("////////////////////////////////////////////////////////////////////////\n");
			printf("Ring_%d\n", i);
			printf("////////////////////////////////////////////////////////////////////////\n");
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
	(void)corosync_cfg_finalize(handle);
	return err;
}

int print_members()
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	const char *membership_key = "runtime.totem.pg.mrp.srp.members.";
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	char prev_key_name[CMAP_KEYNAME_MAXLEN +1 ];
	size_t value_len;
	cmap_value_types_t type;
	int count;

	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//initialize iterator handle (we are looking for members)
	err = cmap_iter_init(cmap_handle, membership_key, &iter_handle);
	if(err != CS_OK){
		printf("Failed to initialize iterator. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	count = -1;
	//print the members
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		if(count < 0 || (unsigned int)atoi(&key_name[strlen(membership_key)]) != (unsigned int)atoi(&prev_key_name[strlen(membership_key)])){
			count++;
			printf("////////////////////////////////////////////////////////////////////////\n");
			printf("%s%d\t%s%u\n", "Node_", count, "ID: ", (unsigned int)atoi(&key_name[strlen(membership_key)]));
			printf("////////////////////////////////////////////////////////////////////////\n");
			strcpy(&prev_key_name[strlen(membership_key)], &key_name[strlen(membership_key)]);
		}
		print_key(cmap_handle, key_name, value_len, NULL ,type);
	}
	//close the handles
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return err;
}
