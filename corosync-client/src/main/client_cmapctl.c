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