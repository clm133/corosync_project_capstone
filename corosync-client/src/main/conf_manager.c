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
	err = get_cmap_value(key_name, NULL, &type);
	if(err != CS_OK){
		return err;
	}
	//switch according to value type (uint32 and string supported)
	switch(type){
			case CMAP_VALUETYPE_UINT32:
				val = malloc(sizeof(uint32_t));
				err = get_cmap_value(key_name, val, &type);
				if(err != CS_OK){
					free(val);
					return err;
				}
				sprintf(&line[strlen(line)], "%u", *(uint32_t *)val);
				free(val);
				break;
			
		case CMAP_VALUETYPE_STRING:
				str = malloc(sizeof(char *));
				err = get_cmap_value(key_name, str, &type);
				if(err != CS_OK){
					free(str);
					return err;
				}
				strcat(line, *str);
				free(*str);
				free(str);
				break;
			
			default:
				return CL_WRONG_CMAP_TYPE;
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
	err = get_cmap_value(key_name, NULL, &t);
	if(err == CS_OK){ 
		//value was found so we can just return
		return CS_OK;
	}
	//if the key/value simply does not exist, we get to set the def_val
	if(err != CS_ERR_NOT_EXIST){
		return err;
	}
	err = set_cmap_value(key_name, def_value, type);
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

int write_node(FILE *fp, int nodeid)
{
	int err;
	int id;
	char line[256];
	char key_buffer[CMAP_KEYNAME_MAXLEN  + 1];
	uint32_t ui32;
	char *str;
	cmap_value_types_t type;
	
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
	err = get_cmap_value(key_buffer, NULL, &type);
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
	err = get_cmap_value(key_buffer, NULL, &type);
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
	Cluster_Member **nodelist;
	
	//get number of nodes (if any)
	err = get_member_count(&total);
	if(err != CS_OK){
		return err;
	}
	if(total <= 0){
		return CS_OK;
	}
	//allocate an array to hold a nodelist
	nodelist = malloc(total * sizeof(Cluster_Member *));
	for(i = 0; i < total; i++){
		nodelist[i] = malloc(sizeof(Cluster_Member));
	}
	//get members of nodelist
	err = get_members(nodelist, total);
	if(err != CS_OK){
		for(i = 0; i < total; i++){
			free(nodelist[i]);
		}
		free(nodelist);
		return err;
	}
	//opening nodelist directive
	fprintf(fp, "nodelist {\n");
	
	//for each node in nodelist
	for(i = 0; i < total; i++){
		write_node(fp, nodelist[i]->nodeid);
		free(nodelist[i]);
	}
	
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