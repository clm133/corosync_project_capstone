#include "conf_writer.h"
#include "modified_cmapctl.h"
#include "client_errors.h"

cs_error_t write_totem(FILE *fp)
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	char *cluster_name[64];
	char *transport[64];
	const char *totem_start = "totem {\n\tversion: 2\n\tsecauth: off";
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//get cluster_name
	err = cmap_get_string(cmap_handle, "totem.cluster_name", cluster_name);
	//use default if nothing is in place
	if(err == CS_ERR_NAME_NOT_FOUND){
		//default = my_cluster
		strcpy(cluster_name[0], "my_cluster");
	}
	//get transport
	err = cmap_get_string(cmap_handle, "totem.transport", transport);
	//use default if nothing is in place
	if(err == CS_ERR_NAME_NOT_FOUND){
		//default = udpu
		strcpy(transport[0], "udpu");
	}
	////we now have everything we need. finalize handle
	(void)cmap_finalize(cmap_handle);
	//version and secauth currently unmodifiable
	fprintf(fp, "%s\n\tcluster_name: %s\n\ttransport: %s\n}\n", totem_start, cluster_name[0], transport[0]);
	return CS_OK;
}

cs_error_t write_quorum(FILE *fp)
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	char *provider[64];
	const char *quorum_start = "quorum {\n";
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//get quorum.provider
	err = cmap_get_string(cmap_handle, "quorum.provider", provider);
	if(err != CS_OK){
		printf("Failed to get quorum.provider. Error#%d: %s", err, get_error(err));
		return err;
	}
	////we now have everything we need. finalize handle
	(void)cmap_finalize(cmap_handle);
	//version and secauth currently unmodifiable
	fprintf(fp, "%s\n\tprovider: %s\n}\n", quorum_start, provider[0]);
	return CS_OK;
	
}

cs_error_t write_logging(FILE *fp)
{	
	// this function may be expanded later on. right now it just needs to print one thing
	const char *logging_start = "logging {";
	fprintf(fp, "%s\n\tto_syslog: yes\n}\n", logging_start);
	return CS_OK;
}

cs_error_t write_node(FILE *fp, uint32_t id, uint32_t votes, uint32_t epsilon, char *ring0_addr)
{
	fprintf(fp,"\n\tnode {\n\t\tnodeid: %u\n\t\tquorum_votes: %u\n\t\tis_epsilon: %u\n\t\tring0_addr: %s\n\t}", id, votes, epsilon, ring0_addr);
	return CS_OK;
}

cs_error_t write_nodelist(FILE *fp)
{
	cs_error_t err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	size_t value_len;
	cmap_value_types_t type;
	char *addr[64];
	uint32_t id;
	uint32_t votes;
	uint32_t epsilon;
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	const char *generic_key = "nodelist.node.X.";
	const char *nodelist_start = "nodelist {";
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize cmap. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//initialize iterator handle (we are looking for nodelist nodes)
	err = cmap_iter_init(cmap_handle, "nodelist.node.", &iter_handle);
	if(err != CS_OK){
		printf("Failed to initialize iterator. Error#%d: %s\n", err, get_error(err));
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//start nodelist 
	fprintf(fp, "%s\n", nodelist_start);
	//iterate through list
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		//find nodeid key
		if(strcmp(&key_name[strlen(generic_key)], "nodeid") == 0){
			//get nodeid
			err = cmap_get_uint32(cmap_handle, key_name, &id);
		}
		//find quorum_votes
		else if(strcmp(&key_name[strlen(generic_key)], "quorum_votes") == 0){
			//get quorum_votes
			err = cmap_get_uint32(cmap_handle, key_name, &votes);
		}
		//find is_epsilon
		else if(strcmp(&key_name[strlen(generic_key)], "is_epsilon") == 0){
			//get quorum_votes
			err = cmap_get_uint32(cmap_handle, key_name, &epsilon);
		}
		//find ring0_addr
		else if(strcmp(&key_name[strlen(generic_key)], "ring0_addr") == 0){
			//get ring0_addr
			err = cmap_get_string(cmap_handle, key_name, addr);
			//have nodeid and ring0_addr, write node {} to file
			err = write_node(fp, id, votes, epsilon, addr[0]);
		}
	}
	//close nodelist directive
	fprintf(fp, "\n}\n");
	
	//close the handles
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

cs_error_t write_config(const char *file_name)
{
	cs_error_t err;
	FILE *fp;
	
	//open file
	fp = fopen(file_name, "w");
	if(fp == NULL){
		printf("Couldn't open file by that name %s\n", file_name);
		return -1;
	}
	//create totem directive
	err = write_totem(fp);
	if(err != CS_OK){
		return err;
	}
	//create quorum directive
	err = write_quorum(fp);
	if(err != CS_OK){
		return err;
	}
	//create logging directive
	err = write_logging(fp);
	if(err != CS_OK){
		return err;
	}
	//create nodelist directive
	err = write_nodelist(fp);
	if(err != CS_OK){
		return err;
	}
	//everything is written to file!
	fclose(fp);
	return err;
}