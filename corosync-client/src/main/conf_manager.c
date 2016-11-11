#include "conf_manager.h"

int create_conf_line(char *key_name, char *value_name, char *line)
{
	int err;
	//values for determining key type
	cmap_value_types_t type;
	size_t len;
	//our malloc pointers
	char** str;
	void *val;
	
	//line starts with value_name
	memset(line, '\0', sizeof(line));
	strcpy(line, value_name);
	strcat(line, ": ");
	//get key type
	err = client_get_cmap_key_type(key_name, &type, &len);
	if(err != CS_OK){
		return err;
	}
	//if key type is not a string
	if(type != CMAP_VALUETYPE_STRING){
		val = malloc(len);
		err = client_get_cmap_value(key_name, val, type);
		if(err != CS_OK){
			free(val);
			return err;
		}
		switch(type){
			case CMAP_VALUETYPE_UINT32:
				sprintf(&line[strlen(line)], "%u", *(uint32_t *)val);
				free(val);
				break;
			default:
				free(val);
				return CL_WRONG_CMAP_TYPE;
		}
	}
	//if key is a string
	else{
		str = malloc(sizeof(char *));
		err = client_get_cmap_string_value(key_name, str);
		if(err != CS_OK){
			free(str);
			return err;
		}
		strcat(line, *str);
		free(*str);
		free(str);
	}
	//success!
	return CS_OK;
}

/*checks if key is set. if not, default value is set*/
int check_for_key(char *key_name, void *def_value, cmap_value_types_t type)
{
	int err;
	cmap_value_types_t t;
	size_t len;
	
	//first check if key/value is even set
	err = client_get_cmap_key_type(key_name, &t, &len);
	if(err == CS_OK){ 
		//value was found so we can just return
		return CS_OK;
	}
	//if the key/value simply does not exist, we get to set the def_val
	if(err != CS_ERR_NOT_EXIST){
		return err;
	}
	err = client_set_cmap_value(key_name, def_value, type);
	if(err != CS_OK){
		return err;
	}
	//looks like everything is ok!
	return CS_OK;
}

int write_totem(FILE *fp)
{
	int err;
	char line[256];
	uint32_t ui32;
	char *str;
	
	//opening totem directive
	fprintf(fp, "totem {\n");
	
	//version
	ui32 = 2; //default setting for version
	err = check_for_key("totem.version", &ui32, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line("totem.version", "version", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t%s\n", line);
	
	//secauth
	str = "off"; //default setting for secauth
	err = check_for_key("totem.secauth", str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line("totem.secauth", "secauth", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t%s\n", line);
	
	//totem_name
	str = "default"; //default setting for cluster_name
	err = check_for_key("totem.cluster_name", str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line("totem.cluster_name", "cluster_name", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t%s\n", line);
	
	//transport
	str = "udpu"; //default setting for transport
	err = check_for_key("totem.transport", str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line("totem.transport", "transport", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t%s\n", line);
	
	//closing totem directive
	fprintf(fp, "}\n");
	
	return CS_OK;
}

int write_quorum(FILE *fp)
{
	int err;
	char line[256];
	uint32_t ui32;
	char *str;
	
	//opening quorum directive
	fprintf(fp, "quorum {\n");
	
	//provider
	str = "corosync_votequorum"; //default setting for provider
	err = check_for_key("quorum.provider", str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line("quorum.provider", "provider", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t%s\n", line);
	
	//closing quorum directive
	fprintf(fp, "}\n");
	
	return CS_OK;
}

int write_logging(FILE *fp)
{	
	int err;
	char line[256];
	uint32_t ui32;
	char *str;
	
	//opening logging directive
	fprintf(fp, "logging {\n");
	
	//to_syslog
	str = "yes"; //default setting for to_syslog
	err = check_for_key("logging.to_syslog", str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line("logging.to_syslog", "to_syslog", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t%s\n", line);
	
	//closing logging directive
	fprintf(fp, "}\n");
	
	return CS_OK;
}

int write_node(FILE *fp, char *ring0_addr, int nodelist_size)
{
	int err;
	int id;
	char line[256];
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	uint32_t ui32;
	char *str;
	
	//try to find node id first
	err = get_id_from_addr(nodelist_size, &id, ring0_addr);
	if(err != CS_OK){
		return err;
	}
	
	//open node 
	fprintf(fp, "\tnode {\n");
	
	//write nodeid
	err = generate_nodelist_key(key_buffer, (uint32_t)id, "nodeid");
	err = create_conf_line(key_buffer, "nodeid", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t\t%s\n", line);
	
	//write ring0_addr
	err = generate_nodelist_key(key_buffer, (uint32_t)id, "ring0_addr");
	err = create_conf_line(key_buffer, "ring0_addr", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t\t%s\n", line);
	
	//write is_epsilon
	str = "no"; //default setting for is_epsilon
	err = generate_nodelist_key(key_buffer, (uint32_t)id, "is_epsilon");
	err = check_for_key(key_buffer, str, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line(key_buffer, "is_epsilon", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t\t%s\n", line);
	
	//write quorum_votes
	ui32 = 2; //default setting for quorum_votes
	err = generate_nodelist_key(key_buffer, (uint32_t)id, "quorum_votes");
	err = check_for_key(key_buffer, &ui32, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		return err;
	}
	err = create_conf_line(key_buffer, "quorum_votes", line);
	if(err != CS_OK){
		return err;
	}
	fprintf(fp, "\t\t%s\n", line);
	
	//close node
	fprintf(fp, "\t}\n");
	
	//success
	return CS_OK;
}

int write_nodelist(FILE *fp)
{
	int err;
	int total;
	int flag;
	int i;
	char **nodelist;
	
	//get number of nodes (if any)
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		return err;
	}
	if(total <= 0){
		return CS_OK;
	}
	
	//get list of node addresses
	nodelist = malloc(sizeof(char *)*total);
	err = nodelist_get_addr_array(nodelist);
	if(err != CS_OK){
		free(nodelist);
		return err;
	}
	
	//opening nodelist directive
	fprintf(fp, "nodelist {\n");
	
	//for each node in nodelist
	flag = CS_OK;
	for(i = 0; i < total; i++){
		err = write_node(fp, nodelist[i], total);
		if(err != CS_OK){
			flag = err;
		}
		free(nodelist[i]);
	}
	free(nodelist);
	
	//closing  nodelist directive
	fprintf(fp, "}\n");
	
	return flag;
}

int write_conf(char *file_name)
{
	int err;
	FILE *fp;
	
	fp = fopen(file_name, "w");
	if(fp == NULL){
		return CL_FILE_NOT_FOUND;
	}
	err = write_totem(fp);
	if(err != CS_OK){
		fclose(fp);
		return err;
	}
	err = write_quorum(fp);
	if(err != CS_OK){
		fclose(fp);
		return err;
	}
	err = write_logging(fp);
	if(err != CS_OK){
		fclose(fp);
		return err;
	}
	err = write_nodelist(fp);
	if(err != CS_OK){
		fclose(fp);
		return err;
	}
	
	//successful
	fclose(fp);
	return CS_OK;
}