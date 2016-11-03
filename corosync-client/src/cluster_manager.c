#include "cluster_manager.h"
#include "client_errors.h"
#include "modified_cmapctl.h"
#include "modified_cfgtool.h"
#include "conf_writer.h"
#include "ssh_manager.h"

int update_all_members()
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	const char *nodelist_key = "nodelist.node";
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	char prev_key_name[CMAP_KEYNAME_MAXLEN +1 ];
	size_t value_len;
	cmap_value_types_t type;
	char *addr;
	char **nodes;
	int count;
	int i;
	int j;

	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//initialize iterator handle (we are looking for members)
	err = cmap_iter_init(cmap_handle, nodelist_key, &iter_handle);
	if(err != CS_OK){
		printf("Failed to initialize iterator. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	count = 0;
	//count the number of nodes left in nodelist
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		if(strcmp(&key_name[strlen(nodelist_key) + 3], "ring0_addr") == 0){
			count++;
		}
	}
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	//initialize iterator handle (we are looking for node addresses now)
	err = cmap_iter_init(cmap_handle, nodelist_key, &iter_handle);
	if(err != CS_OK){
		printf("Failed to initialize iterator. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//malloc nodes list to hold all addresses
	nodes = malloc(sizeof(char *) * count);
	i = 0;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		if(strcmp(&key_name[strlen(nodelist_key) + 3], "ring0_addr") == 0){
			//malloc address
			addr = NULL;
			nodes[i] = malloc(value_len * sizeof(char));
			for(j = 0; j < value_len; j++){
				nodes[i][j] = (char)0;
			}
			cmap_get_string(cmap_handle, key_name, &addr);
			strncpy(nodes[i], addr, value_len - 1);
			i++;
			free(addr);
		}
	}
	//finally move through our list. shutting off corosync, sftp-ing the new conf file, and starting corosync up again
	for(i = 0; i < count; i++){
		printf("updating node %s...", nodes[i]);
		copy_conf(nodes[i], "corosync.conf");
		stop_corosync(nodes[i]);
		start_corosync(nodes[i]);
		printf("successful!\n");
		free(nodes[i]);
	}
	free(nodes);
	//close the handles
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return 1;
}

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
	char set_id_key[128];
	char set_votes_key[128];
	char set_epsilon_key[128];
	char set_ring0_key[128];
	char id_char[32];
	char *nodelist = "nodelist.node.";
	char *set_id = ".nodeid";
	char *set_votes = ".quorum_votes";
	char *set_epsilon = ".is_epsilon";
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
	//key = nodelist.X.node.quorum_votes
	strcpy(set_votes_key, nodelist);
	strcat(set_votes_key, id_char);
	strcat(set_votes_key, set_votes);
	//key = nodelist.X.node.is_epsilon
	strcpy(set_epsilon_key, nodelist);
	strcat(set_epsilon_key, id_char);
	strcat(set_epsilon_key, set_epsilon);
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
	cmap_set_uint32(cmap_handle, set_votes_key, 2);
	cmap_set_string(cmap_handle, set_epsilon_key, "no");
	cmap_set_string(cmap_handle, set_ring0_key, addr);
	(void)cmap_finalize(cmap_handle);
	//write to conf file
	err = write_config("corosync.conf");
	err = update_all_members();
	return err;
}

// remove node
// we should know the id to remove the node we want
// name the address "key" for convenience.
int remove_node(uint32_t id){

	cs_error_t err;
	cmap_handle_t cmap_handle;
	char id_addr[128];
	char id_key[128];
	char votes_key[128];
	char epsilon_key[128];
	char id_char[32];
	char *nodelist = "nodelist.node.";
	char *set_id = ".nodeid";
	char *set_votes = ".quorum_votes";
	char *set_epsilon = ".is_epsilon";
	char *set_addr = ".ring0_addr";
	char removeAddr[INET6_ADDRSTRLEN];
	unsigned int id_check;
	unsigned int votes_check;
	char *epsilon_check;
	int i;
	for(i = 0; i < 128; i++){
		id_addr[i] = (char)0;
	}
	for(i = 0; i < 128; i++){
		id_key[i] = (char)0;
	}
	for(i = 0; i < 128; i++){
		votes_key[i] = (char)0;
	}
	for(i = 0; i < 128; i++){
		epsilon_key[i] = (char)0;
	}
	for(i = 0; i < 32; i++){
		id_char[i] = (char)0;
	}
	sprintf(id_char, "%u", ((unsigned int)id) - 1);
	
	//key = nodelist.X.node.nodeid
	strcpy(id_key, nodelist);
	strcat(id_key, id_char);
	strcat(id_key, set_id);
	strcpy(votes_key, nodelist);
	strcat(votes_key, id_char);
	strcat(votes_key, set_votes);
	strcpy(epsilon_key, nodelist);
	strcat(epsilon_key, id_char);
	strcat(epsilon_key, set_epsilon);
	strcat(id_addr, nodelist);
	strcat(id_addr, id_char);
	strcat(id_addr, set_addr);
	
	// initialize cmap_handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return -1;
	}
	//save ip addr of node we are removing for later
	err = get_addr((int)id, removeAddr);
	if(err == -1){
		printf("Failed to find node\n");
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	
	// get key, catch error
	err = cmap_get_uint32(cmap_handle, id_key, &id_check);
	if(err != CS_OK){
		printf("Failed to retrieve key. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	
	// get votes, catch error
	err = cmap_get_uint32(cmap_handle, votes_key, &votes_check);
	if(err != CS_OK){
		printf("Failed to retrieve key. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	
	// get epsilon, catch error
	err = cmap_get_string(cmap_handle, epsilon_key, &epsilon_check);
	if(err != CS_OK){
		printf("Failed to retrieve key. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	
	// the key exists, delete that node
	err = cmap_delete(cmap_handle,id_addr);
	if(err != CS_OK){
		printf("Failed to delete key:%s. Error#%d: %s\n", id_addr, err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	err = cmap_delete(cmap_handle, id_key);
	if(err != CS_OK){
		printf("Failed to delete key:%s. Error#%d: %s\n", id_key, err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	err = cmap_delete(cmap_handle, votes_key);
	if(err != CS_OK){
		printf("Failed to delete key:%s. Error#%d: %s\n", votes_key, err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	err = cmap_delete(cmap_handle, epsilon_key);
	if(err != CS_OK){
		printf("Failed to delete key:%s. Error#%d: %s\n", epsilon_key, err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return -1;
	}
	
	// finalize
	(void)cmap_finalize(cmap_handle);
	//shut off removed node
	printf("Removing and shutting off node %s...", removeAddr);
	copy_conf(removeAddr, "../conf_templates/default.conf");
	stop_corosync(removeAddr);
	printf("success!\n");
	//write to conf file
	err = write_config("corosync.conf");
	err = update_all_members();
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
		printf("%s\t%s%u\n", "local_node", "ID: ", node_id);
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
	char addr[INET6_ADDRSTRLEN];
	char joined[32];
	char *str;
	const char *membership_key = "runtime.totem.pg.mrp.srp.members.";
	const char *joined_key = "runtime.totem.pg.mrp.srp.members.";
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	char prev_key_name[CMAP_KEYNAME_MAXLEN +1 ];
	size_t value_len;
	cmap_value_types_t type;
	int count;
	int i;

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
	printf("PRINTING MEMBERSHIP STATUS:\n");
	//print the members
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		if(count < 0 || (unsigned int)atoi(&key_name[strlen(membership_key)]) != (unsigned int)atoi(&prev_key_name[strlen(membership_key)])){
			count++;
			get_addr(atoi(&key_name[strlen(membership_key)]), addr);
			printf("////////////////////////////////////////////////////////////////////////\n");
			printf("%s%u\n", "Node_", (unsigned int)atoi(&key_name[strlen(membership_key)])); 
			printf("////////////////////////////////////////////////////////////////////////\n");
			printf("%s%u\n", "ID: ", (unsigned int)atoi(&key_name[strlen(membership_key)]));
			printf("%s%s\n", "IP_ADDR: ", addr);
			strcpy(&prev_key_name[strlen(membership_key)], &key_name[strlen(membership_key)]);
		}
		if(strcmp(&key_name[strlen(joined_key) + 2], "status") == 0){
			str = NULL;
			for(i = 0; i < 32; i++){
				joined[i] = (char)0;
			}
			cmap_get_string(cmap_handle, key_name, &str);
			strncpy(joined, str, 31);
			free(str);
			printf("STATUS: %s\n", joined);
		}
	}
	//close the handles
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return err;
}

int reset_cluster(char *path_to_conf)
{
	uint32_t highest_id;
	unsigned int id_to_remove;
	char addr[INET6_ADDRSTRLEN];
	int i;
	
	for(i = 0; i < INET6_ADDRSTRLEN; i++){
			addr[i] = (char)0;
		}
	get_highest_node(&highest_id);
	while(highest_id != 0){
		get_addr((int)highest_id, addr);
		stop_corosync(addr);
		copy_conf(addr, path_to_conf);
		printf("Reset nodeid %u\n", (unsigned int)highest_id);
		highest_id--;
		for(i = 0; i < INET6_ADDRSTRLEN; i++){
			addr[i] = (char)0;
		}
	}
	/*
	get_addr(highest_id, addr);
	stop_corosync(addr);
	copy_conf(addr, path_to_conf);
	*/
	return 1;
}
