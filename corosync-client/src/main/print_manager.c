#include "print_manager.h"

int print_membership()
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
	return CS_OK;
}

int print_quorum()
{	
	return CS_OK;
}