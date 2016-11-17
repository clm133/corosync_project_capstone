#include "client_cmapctl.h"

int client_set_cmap_value(const char *key_name, void *value, cmap_value_types_t type)
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

int client_get_cmap_value(const char *key_name, void *value, cmap_value_types_t type)
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
	cs_err = cmap_get(cmap_handle, key_name, NULL, &len, &type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//get key from cmap
	//if value is a string, this function doesn't deal with that
	if(type == CMAP_VALUETYPE_STRING){
		(void)cmap_finalize(cmap_handle);
		return CL_WRONG_CMAP_TYPE;
	}
	else{
		cs_err = cmap_get(cmap_handle, key_name, value, &len, &type);
		if(cs_err != CS_OK){
			(void)cmap_finalize(cmap_handle);
			return cs_err;
		}
	}
	//hey look we were successful
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int client_get_cmap_string_value(const char *key_name, char **value)
{
	cs_error_t cs_err;
	cmap_handle_t cmap_handle;
	size_t len;
	cmap_value_types_t type;

	//initialize cmap
	cs_err = cmap_initialize(&cmap_handle);
	if(cs_err != CS_OK){
		return cs_err;
	}
	//check if key exists in cmap
	cs_err = cmap_get(cmap_handle, key_name, NULL, &len, &type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//check type
	if(type != CMAP_VALUETYPE_STRING){
		(void)cmap_finalize(cmap_handle);
		return CL_WRONG_CMAP_TYPE;
	}
	//get key from cmap
	cs_err = cmap_get_string(cmap_handle, key_name, value);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//hey look we were successful
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int client_delete_cmap_value(const char *key_name, void *value, cmap_value_types_t type)
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
	cs_err = cmap_get(cmap_handle, key_name, NULL, &len, &type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//if the caller wants us to return the deleted value
	if(value != NULL){
		//get key from cmap
		cs_err = cmap_get(cmap_handle, key_name, value, &len, &type);
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

int client_delete_cmap_string_value(const char *key_name, char **value)
{
	cs_error_t cs_err;
	cmap_handle_t cmap_handle;
	size_t len;
	cmap_value_types_t type;
	
	//initialize cmap
	cs_err = cmap_initialize(&cmap_handle);
	if(cs_err != CS_OK){
		return cs_err;
	}
	//check if key exists in cmap
	cs_err = cmap_get(cmap_handle, key_name, NULL, &len, &type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//check type
	if(type != CMAP_VALUETYPE_STRING){
		(void)cmap_finalize(cmap_handle);
		return CL_WRONG_CMAP_TYPE;
	}
	//if the caller wants us to return the deleted value
	if(value != NULL){
		//get key from cmap
		cs_err = cmap_get_string(cmap_handle, key_name, value);
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

int client_get_cmap_key_type(const char *key_name, cmap_value_types_t *type, size_t *len)
{
	cs_error_t cs_err;
	cmap_handle_t cmap_handle;
	
	//initialize cmap
	cs_err = cmap_initialize(&cmap_handle);
	if(cs_err != CS_OK){
		return cs_err;
	}
	//check if key exists in cmap
	cs_err = cmap_get(cmap_handle, key_name, NULL, len, type);
	if(cs_err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return cs_err;
	}
	//hey look we were successful
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int nodelist_get_total(int *total)
{
	int err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1]; //this stores the key name found by the iterator
	cmap_value_types_t type;
	size_t len;
	const char *generic_id_key = "nodelist.node.X.";
	int count;
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		return err;
	}
	//initialize iterator handle (we are looking for all nodeid)
	err = cmap_iter_init(cmap_handle, "nodelist.node.", &iter_handle);
	if(err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//count nodes in iterator
	count = 0;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &len, &type)) == CS_OK){
		//find nodeid key
		if(strcmp(&key_name[strlen(generic_id_key)], "nodeid") == 0){
			count++;
		}
	}
	*total = count;
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int nodelist_get_id_array(uint32_t **list)
{
	int err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1]; //this stores the key name found by the iterator
	cmap_value_types_t type;
	size_t len;
	const char *generic_id_key = "nodelist.node.X.";
	int i;
	int j;
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		return err;
	}
	//initialize iterator handle (we are looking for all nodeid)
	err = cmap_iter_init(cmap_handle, "nodelist.node.", &iter_handle);
	if(err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	i = 0;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &len, &type)) == CS_OK){
		//find nodeid key
		if(strcmp(&key_name[strlen(generic_id_key)], "nodeid") == 0){
			list[i] = malloc(sizeof(uint32_t));
			err = client_get_cmap_value(key_name, list[i], CMAP_VALUETYPE_UINT32);
			i++;
			if(err != CS_OK){
				break;
			}
		}
	}
	if(err != CS_OK && err != CS_ERR_NO_SECTIONS){
		//if there was some kind of error we need to free anything we've malloc'd up until that point
		for(j = 0; j < i; j++){
			free(list[j]);
		}
		(void)cmap_iter_finalize(cmap_handle, iter_handle);
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//hey look! we were successful!
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}

int nodelist_get_addr_array(char **list)
{
	int err;
	cmap_handle_t cmap_handle;
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1]; //this stores the key name found by the iterator
	cmap_value_types_t type;
	size_t len;
	const char *generic_id_key = "nodelist.node.X.";
	int i;
	int j;
	
	//initialize cmap handle
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		return err;
	}
	//initialize iterator handle (we are looking for all ring0_addr)
	err = cmap_iter_init(cmap_handle, "nodelist.node.", &iter_handle);
	if(err != CS_OK){
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	i = 0;
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &len, &type)) == CS_OK){
		//find nodeid key
		if(strcmp(&key_name[strlen(generic_id_key)], "ring0_addr") == 0){
			err = client_get_cmap_string_value(key_name, &list[i]);
			i++;
			if(err != CS_OK){
				break;
			}
		}
	}
	if(err != CS_OK && err != CS_ERR_NO_SECTIONS){
		//if there was some kind of error we need to free anything we've malloc'd up until that point
		for(j = 0; j < i; j++){
			free(list[j]);
		}
		(void)cmap_iter_finalize(cmap_handle, iter_handle);
		(void)cmap_finalize(cmap_handle);
		return err;
	}
	//hey look! we were successful!
	(void)cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}