#include "cluster_manager.h"

/* helper function for add_node() and delete_node(), generates a key prefix "nodelist.node.X."*/
int generate_nodelist_key(char *key_buffer, uint32_t id, char *key_suffix)
{
	int err; //There may be some room in this function to add some error checking

	//set up our key buffers
	memset(key_buffer, '\0', sizeof(key_buffer));
	//copy first half of key
	strcpy(key_buffer, "nodelist.node.");
	//convert unique id to char and add to key buffers
	sprintf(&key_buffer[strlen(key_buffer)], "%u.", id);
	//append suffix and we are good!
	strcat(key_buffer, key_suffix);
	//There may be some room in this function to add some error checking
	return CS_OK;
}

/* Frees any unit32** nodelist we might generate */
void free_uint32_nodelist(uint32_t **nodelist, int nodelist_size)
{
	int i;

	for(i = 0; i < nodelist_size; i++){
		free(nodelist[i]);
	}
	free(nodelist);
}

/* finds unique ID given nodelist */
int next_availible_id(int nodelist_size, int *id)
{
	uint32_t **nodelist;
	int err;
	int i;
	int j;
	
	//get an array of all of those nodes's IDs
	nodelist = malloc(sizeof(uint32_t *) * nodelist_size);
	err = nodelist_get_id_array(nodelist);
	if(err != CS_OK){
		free_uint32_nodelist(nodelist, nodelist_size);
		return err;
	}
	//using our array, find an availible node id
	for(i = 0; i <= MAX_NODES_SUPPORTED; i++){ //i is an incrementing int, which is compared for uniqueness against each id in nodelist[j]
		j = 0;
		*id = 0;
		while(j < nodelist_size){
			if((unsigned int)i == *nodelist[j]){ //this will be equal if id i is taken already
				*id = -1; //the -1 represents a flag for later in the enclosing for-loop
				break;
			}
			j++;
		}
		if(*id != -1 && i <= MAX_NODES_SUPPORTED){ //if we have found a unique id, we can now make a key!
			*id = i;
			break; //so lets break!
		}
	}
	//no matter what, we are done with nodelist, so lets free all of it
	free_uint32_nodelist(nodelist, nodelist_size);
	//make sure our ID is legit
	if(*id < 0 || *id > MAX_NODES_SUPPORTED){
		return CL_NODEID_ERR;
	}
	return CS_OK;
}

/* finds node id of given node addr */
int get_id_from_addr(int nodelist_size, uint32_t *id, char *addr)
{
	uint32_t **nodelist;
	int err;
	int i;
	int flag;
	char addr_key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	char **str;
	
	flag = 0;
	//get an array of all of the node IDs
	nodelist = malloc(sizeof(uint32_t *) * nodelist_size);
	err = nodelist_get_id_array(nodelist);
	if(err != CS_OK){
		free_uint32_nodelist(nodelist, nodelist_size);
		return err;
	}
	
	//for each node id in the array
	for(i = 0; i < nodelist_size; i++){
		//create a nodelist.node.X.ring0_addr key from nodelist member
		generate_nodelist_key(&addr_key_buffer[0], *(uint32_t *)nodelist[i], "ring0_addr");
		//check the value of that key against the addr parameter
		str = malloc(sizeof(char *));
		err = client_get_cmap_string_value(addr_key_buffer, str);
		if(err == CS_OK){
			if(strcmp(*str, addr) == 0){
				*id = *(int *)nodelist[i];
				flag = 1; //just setting a flag to let the function know we had a success.
			}
			free(*str);
		}
		free(str);
	}
	//free our nodelist
	free_uint32_nodelist(nodelist, nodelist_size);
	//check if we were successful
	if(flag != 1){
		return CL_NODEID_ERR;
	}
	return CS_OK;
}

int add_node(char *addr)
{
	int err;
	int new_id;
	uint32_t ui32;
	char *str;
	//this buffer is where we will construct our cmap keys
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	
	// the total number of nodes is used to determine the next availible unique nodeid in our cluster
	int total;
	
//get the total number of nodes in the cluster
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		return err;
	}
	
//get the next availible unique nodeid in the cluster
	if(total >= 1){
		err = next_availible_id(total, &new_id);
		if(err != CS_OK){
		return err;	
		}
	}
	else{
		new_id = 0; //if we find no nodes in the nodelist, 0 will simply be our first ID
	}
	
//use the unique node id to generate the new node's cmap keys and set their values in the cmap
	//nodelist.node.X.nodeid
	err = generate_nodelist_key(&key_buffer[0], (uint32_t)new_id, "nodeid");
	err = client_set_cmap_value(key_buffer, &new_id, CMAP_VALUETYPE_UINT32);  // always set nodeid before ring0_addr
	if(err != CS_OK){
		return err;
	}
	//nodelist.node.X.ring0_addr
	err = generate_nodelist_key(&key_buffer[0], (uint32_t)new_id, "ring0_addr");
	err = client_set_cmap_value(key_buffer, addr, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	//nodelist.node.X.is_epsilon
	err = generate_nodelist_key(&key_buffer[0], (uint32_t)new_id, "is_epsilon");
	str = "no";
	err = client_set_cmap_value(key_buffer, str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	//nodelist.node.X.quorum_votes
	err = generate_nodelist_key(&key_buffer[0], (uint32_t)new_id, "quorum_votes");
	ui32 = 2;
	err = client_set_cmap_value(key_buffer, &ui32, CMAP_VALUETYPE_UINT32);  
	if(err != CS_OK){
		return err;
	}
	
//if successful, return CS_OK
	return CS_OK;
}

int delete_node(char *addr)
{
	int err;
	uint32_t del_id;
	//these buffers are where we will construct our cmap keys
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	// the total number of nodes is used to help navigating any arrays we build
	int total;
	
//get the total number of nodes in the cluster
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		return err;
	}
	
//search the cluster for matching addr
	err = get_id_from_addr(total, &del_id, addr);
	if(err != CS_OK){
		return err;
	}
	
//generate cmap keys and delete from cmap
	//nodelist.node.X.ring0_addr
	err = generate_nodelist_key(&key_buffer[0], del_id, "ring0_addr");
	err = client_delete_cmap_string_value(key_buffer, NULL);
	if(err != CS_OK){
		return err;
	}
	//nodelist.node.X.is_epsilon
	err = generate_nodelist_key(&key_buffer[0], del_id, "is_epsilon");
	err = client_delete_cmap_string_value(key_buffer, NULL);
	if(err != CS_OK){
		return err;
	}
	//nodelist.node.X.quorum_votes
	err = generate_nodelist_key(&key_buffer[0], del_id, "quorum_votes");
	err = client_delete_cmap_value(key_buffer, NULL, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		return err;
	}
	//nodelist.node.X.nodeid
	err = generate_nodelist_key(&key_buffer[0], del_id, "nodeid");
	err = client_delete_cmap_value(key_buffer, NULL, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		return err;
	}
	
//if successful, return CS_OK
	return CS_OK;
}

int move_epsilon(uint32_t e_id, uint32_t ex_id)
{
	int err;
	
	err = remove_epsilon(ex_id);
	if(err != CS_OK){
		return err;
	}
	err = set_epsilon(e_id);
	if(err != CS_OK){
		return err;
	}
	
	return CS_OK;
}

int set_node_votes(uint32_t id, uint32_t votes)
{
	int err;
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	
	err = generate_nodelist_key(key_buffer, id, "quorum_votes");
	err = client_set_cmap_value(key_buffer, &votes, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		return err;
	}
	return CS_OK;
}

int set_epsilon(uint32_t e_id)
{
	int err;
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	char *str = "yes";
	
	err = generate_nodelist_key(key_buffer, e_id, "is_epsilon");
	err = client_set_cmap_value(key_buffer, str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	return CS_OK;
}

int remove_epsilon(uint32_t e_id)
{
	int err;
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	char *str = "no";
	
	err = generate_nodelist_key(key_buffer, e_id, "is_epsilon");
	err = client_set_cmap_value(key_buffer, str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	return CS_OK;
}