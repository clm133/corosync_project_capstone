#include "cluster_manager.h"
#include "client_errors.h"

#define MEMBERS_PREFIX "runtime.totem.pg.mrp.srp.members."
#define MAX_TRY_AGAIN 10

int add_node(char *addr)
{
	return 0;
}

/* I pretty much just lifted the print_key function from the corosync-cmapctl here */
int print_key(cmap_handle_t handle, const char *key_name, size_t value_len, const void *value, cmap_value_types_t type)
{
	char *str;
	char *bin_value;
	cs_error_t err;
	int8_t i8;
	uint8_t u8;
	int16_t i16;
	uint16_t u16;
	int32_t i32;
	uint32_t u32;
	int64_t i64;
	uint64_t u64;
	float flt;
	double dbl;
	int end_loop;
	int no_retries;
	size_t bin_value_len;

	end_loop = 0;
	no_retries = 0;

	err = CS_OK;

	while(!end_loop){
		switch(type){
			case CMAP_VALUETYPE_INT8:
				if(value == NULL){
					err = cmap_get_int8(handle, key_name, &i8);
				}
				else{
					i8 = *((int8_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_INT16:
				if(value == NULL){
					err = cmap_get_int16(handle, key_name, &i16);
				}
				else{
					i16 = *((int16_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_INT32:
				if(value == NULL){
					err = cmap_get_int32(handle, key_name, &i32);
				}
				else{
					i32 = *((int32_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_INT64:
				if(value == NULL){
					err = cmap_get_int64(handle, key_name, &i64);
				} 
				else{
					i64 = *((int64_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_UINT8:
				if(value == NULL){
					err = cmap_get_uint8(handle, key_name, &u8);
				} 
				else{
					u8 = *((uint8_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_UINT16:
				if(value == NULL){
					err = cmap_get_uint16(handle, key_name, &u16);
				}
				else{
					u16 = *((uint16_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_UINT32:
				if(value == NULL){
					err = cmap_get_uint32(handle, key_name, &u32);
				}
				else{
					u32 = *((uint32_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_UINT64:
				if(value == NULL){
					err = cmap_get_uint64(handle, key_name, &u64);
				}
				else{
					u64 = *((uint64_t *)value);
				}
				break;
				
			case CMAP_VALUETYPE_FLOAT:
				if(value == NULL){
					err = cmap_get_float(handle, key_name, &flt);
				} 
				else{
					flt = *((float *)value);
				}
				break;
				
			case CMAP_VALUETYPE_DOUBLE:
				if(value == NULL){
					err = cmap_get_double(handle, key_name, &dbl);
				}
				else{
					dbl = *((double *)value);
				}
				break;
				
			case CMAP_VALUETYPE_STRING:
				if (value == NULL) {
					err = cmap_get_string(handle, key_name, &str);
				}
				else{
					str = (char *)value;
				}
				break;
				
			case CMAP_VALUETYPE_BINARY:
				bin_value = (char *)value;
				break;
		}
		//
		if(err == CS_OK){
			end_loop = 1;
		}
		else if(err == CS_ERR_TRY_AGAIN){
			sleep(1);
			no_retries++;
			if(no_retries > MAX_TRY_AGAIN){
				end_loop = 1;
			}
		}
		else{
			end_loop = 1;
		}
	}; //end of while()

	if(err != CS_OK){
		printf("Can't get value of %s. Error %s\n", key_name, get_error(err));
		return err;
	}
	printf("%s (", key_name);
	
	switch(type){
		case CMAP_VALUETYPE_INT8:
			printf("%s) = %"PRId8, "i8", i8);
			break;
			
		case CMAP_VALUETYPE_UINT8:
			printf("%s) = %"PRIu8, "u8", u8);
			break;
			
		case CMAP_VALUETYPE_INT16:
			printf("%s) = %"PRId16, "i16", i16);
			break;
			
		case CMAP_VALUETYPE_UINT16:
			printf("%s) = %"PRIu16, "u16", u16);
			break;
			
		case CMAP_VALUETYPE_INT32:
			printf("%s) = %"PRId32, "i32", i32);
			break;
			
		case CMAP_VALUETYPE_UINT32:
			printf("%s) = %"PRIu32, "u32", u32);
			break;
			
		case CMAP_VALUETYPE_INT64:
			printf("%s) = %"PRId64, "i64", i64);
			break;
			
		case CMAP_VALUETYPE_UINT64:
			printf("%s) = %"PRIu64, "u64", u64);
			break;
			
		case CMAP_VALUETYPE_FLOAT:
			printf("%s) = %f", "flt", flt);
			break;
			
		case CMAP_VALUETYPE_DOUBLE:
			printf("%s) = %lf", "dbl", dbl);
			break;
			
		case CMAP_VALUETYPE_STRING:
			printf("%s) = %s", "str", str);
			if (value == NULL) {
				free(str);
			}
			break;
			
		case CMAP_VALUETYPE_BINARY:
			printf("%s)", "bin");
			printf("*empty*");
			break;
	}
	printf("\n");
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
	unsigned int j;
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
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	char *cur_key;
	char prev_key_name[CMAP_KEYNAME_MAXLEN + 1];
	char *old_key;
	size_t value_len;
	cmap_value_types_t type;
	int count;
	
	
	printf("PRINTING MEMBERSHIP STATUS:\n");
	//try to initialize cmap
	err = cmap_initialize(&cmap_handle);
	if(err != CS_OK){
		printf("Failed to initialize iteration. Error#%d: %s\n", err, get_error(err));
		return err;
	}
	//try to initialize iterator
	err = cmap_iter_init(cmap_handle, MEMBERS_PREFIX, &iter_handle);
	if (err != CS_OK) {
		printf("Failed to initialize iteration. Error: %s\n", get_error(err));
		return err;
	}
	count = -1;
	//basically a while(hasNext())
	while((err = cmap_iter_next(cmap_handle, iter_handle, key_name, &value_len, &type)) == CS_OK){
		cur_key = &key_name[0 + strlen(MEMBERS_PREFIX)];
		if(count < 0 || (unsigned int)atoi(cur_key) != (unsigned int)atoi(old_key)){
			count++;
			printf("////////////////////////////////////////////////////////////////////////\n");
			printf("%s%d\t%s%u\n", "Node_", count, "ID: ", (unsigned int)atoi(cur_key)); 
			printf("////////////////////////////////////////////////////////////////////////\n");
			old_key = cur_key;
		}
		print_key(cmap_handle, key_name, value_len, NULL, type);
	}
	cmap_iter_finalize(cmap_handle, iter_handle);
	(void)cmap_finalize(cmap_handle);
	return CS_OK;
}