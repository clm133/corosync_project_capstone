#include "client_cmapctl.h"

int set_cmap_value(const char *key_name, void *value, cmap_value_types_t type)
{
	cs_error_t cs_err;
	cmap_handle_t cmap_handle;
	
	
	//find out if type is acceptable
	if(type < 1 || type > 11){
		return CL_WRONG_CMAP_TYPE;
	}
	//initialize cmap
	cs_err = cmap_initialize(&cmap_handle);
	if(cs_err != CS_OK){
		return cs_err;
	}
	//set according to value type
	switch(type){
		//int8 values
		case CMAP_VALUETYPE_INT8:
			cs_err = cmap_set_int8(cmap_handle, key_name, *(int8_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//uint8 values
		case CMAP_VALUETYPE_UINT8:
			cs_err = cmap_set_uint8(cmap_handle, key_name, *(uint8_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//int16 values
		case CMAP_VALUETYPE_INT16:
			cs_err = cmap_set_int16(cmap_handle, key_name, *(int16_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//uint16 values
		case CMAP_VALUETYPE_UINT16:
			cs_err = cmap_set_uint16(cmap_handle, key_name, *(uint16_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
				}
			break;
		//int32 values
		case CMAP_VALUETYPE_INT32:
			cs_err = cmap_set_int32(cmap_handle, key_name, *(int32_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
				}
			break;
		//uint32 values
		case CMAP_VALUETYPE_UINT32:
			cs_err = cmap_set_uint32(cmap_handle, key_name, *(uint32_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//int64 values
		case CMAP_VALUETYPE_INT64:
			cs_err = cmap_set_int64(cmap_handle, key_name, *(int64_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//uint64 values
		case CMAP_VALUETYPE_UINT64:
			cs_err = cmap_set_uint64(cmap_handle, key_name, *(uint64_t *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//float values
		case CMAP_VALUETYPE_FLOAT:
			cs_err = cmap_set_float(cmap_handle, key_name, *(float *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//double values
		case CMAP_VALUETYPE_DOUBLE:
			cs_err = cmap_set_double(cmap_handle, key_name, *(double *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//string
		case CMAP_VALUETYPE_STRING:
			cs_err = cmap_set_string(cmap_handle, key_name, (char *)value);
			if(cs_err != CS_OK){
				(void)cmap_finalize(cmap_handle);
				return cs_err;
			}
			break;
		//that's strange...doesn't look like the type matches anything...
		default:
			(void)cmap_finalize(cmap_handle);
			return CL_WRONG_CMAP_TYPE;
	}
	//if we made it here, key was set successfully.
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int get_cmap_value(const char *key_name, void *value, cmap_value_types_t *type)
{
	cs_error_t cs_err;
	cmap_handle_t cmap_handle;
	size_t len;
	
	//initialize cmap
	cs_err = cmap_initialize(&cmap_handle);
	if(cs_err != CS_OK){
		return cs_err;
	}
	//check if key exists in cmap
	cs_err = cmap_get(cmap_handle, key_name, NULL, &len, type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//if this function was called with null, we just want to return and confirm key was in place
	if(value == NULL){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//otherwise, we wnt to get key from cmap
	if(*type == CMAP_VALUETYPE_STRING){
		cs_err = cmap_get_string(cmap_handle, key_name, value);
	}
	else{
		cs_err = cmap_get(cmap_handle, key_name, value, &len, type);
	}
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//hey look we were successful
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int delete_cmap_value(const char *key_name, void *value, cmap_value_types_t *type)
{
	cs_error_t cs_err;
	cmap_handle_t cmap_handle;
	size_t len;
	
	//initialize cmap
	cs_err = cmap_initialize(&cmap_handle);
	if(cs_err != CS_OK){
		return cs_err;
	}
	//check if key exists in cmap
	cs_err = cmap_get(cmap_handle, key_name, NULL, &len, type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//if the caller wants us to return the deleted value
	if(value != NULL){
		if(*type == CMAP_VALUETYPE_STRING){
			cs_err = cmap_get_string(cmap_handle, key_name, value);
		}
		else{
			cs_err = cmap_get(cmap_handle, key_name, value, &len, type);
		}
	}
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//actually delete the key from cmap
	cs_err = cmap_delete(cmap_handle, key_name);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//hey look we were successful
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int get_member_count(int *count)
{
	int err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1]; //this stores the key name found by the iterator
	cmap_value_types_t type;
	size_t len;
	const char *generic_key_prefix = "runtime.totem.pg.mrp.srp.members.";
	int total;
	int prev;
	int id;
	int i;
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		return err;
	}
	//initialize iterator handle (we are looking for all nodeid)
	err = cmap_iter_init(cmap_handle, generic_key_prefix, &iter_handle);
	if(err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//count nodes in iterator
	prev = -1;
	total = 0;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &len, &type)) == CS_OK){
		//find a unique node id
		sscanf(key_name, "runtime.totem.pg.mrp.srp.members.%d", &id);
		if(id != prev){
			prev = id;
			total++;
		}
	}
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	*count = total;
	return CS_OK;
}

int get_members(Cluster_Member **array, int array_len)
{
	int err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1]; //this stores the key name found by the iterator
	cmap_value_types_t type;
	size_t len;
	char **str;
	char key_suffix[64]; //no key suffix is going to be larger than this
	const char *generic_key_prefix = "runtime.totem.pg.mrp.srp.members.";
	int total;
	int prev;
	int id;
	int i;
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		return err;
	}
	//initialize iterator handle (we are looking for all nodeid)
	err = cmap_iter_init(cmap_handle, generic_key_prefix, &iter_handle);
	if(err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//count nodes in iterator
	prev = -1;
	i = -1;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &len, &type)) == CS_OK){
		//find a unique node id
		memset(key_suffix, sizeof(key_suffix), '\0');
		sscanf(key_name, "runtime.totem.pg.mrp.srp.members.%d.%s", &id, key_suffix);
		if(id != prev){
			prev = id;
			i++;
			array[i]->nodeid = id;
		}
		if(strcmp(key_suffix, "ip") == 0){
			memset(array[i]->ip, sizeof(array[i]->ip), '\0');
			str = malloc(sizeof(char *));
			err = get_cmap_value(key_name, str, &type);
			if(err == CS_OK){
				strcpy(array[i]->ip, *str);
				free(*str);
			}
			free(str);
		}
		else if(strcmp(key_suffix, "status") == 0){
			memset(array[i]->status, sizeof(array[i]->status), '\0');
			str = malloc(sizeof(char *));
			err = get_cmap_value(key_name, str, &type);
			if(err == CS_OK){
				strcpy(array[i]->status, *str);
				free(*str);
			}
			free(str);
		}
	}
	
	//success
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

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

int generate_members_key(char *key_buffer, uint32_t id, char *key_suffix)
{
	int err; //There may be some room in this function to add some error checking

	//set up our key buffers
	memset(key_buffer, '\0', sizeof(key_buffer));
	//copy first half of key
	strcpy(key_buffer, "runtime.totem.pg.mrp.srp.members.");
	//convert unique id to char and add to key buffers
	sprintf(&key_buffer[strlen(key_buffer)], "%u.", id);
	//append suffix and we are good!
	strcat(key_buffer, key_suffix);
	//There may be some room in this function to add some error checking
	return CS_OK;
}