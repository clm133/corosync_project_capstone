#include "client_cmap.h"

int cmap_init(cmap_handle_t conn)
{
	int err;
	err = cmap_initialize(&conn);
	printf("cmap initialized...\n");
	return err;
}

int cmap_close(cmap_handle_t conn)
{
	int err;
	cmap_finalize(conn);
	printf("cmap closed...\n");
	return 0;
}

int print_membership()
{
	struct node_info{
		uint32_t node_id;
        char node_name[128];
    }nodes[4]; //only 4 for now. can change to support more in the future
	int err;
	cmap_handle_t conn; 
	cmap_iter_handle_t iter_h;
	uint32_t node_id ;
	
	//initialize connection and iterator
	err = cmap_init(conn);
	if(err != CS_OK){
		printf("Could not establish connection. Error #: %s\n", get_error(err));
		return err;
	}
	err = cmap_iter_init(conn, "nodelist.node", &iter_h);
	if(err != CS_OK){
		printf("Could not establish cmap iterator\n");
		return err;
	}
	while(1){
		char key_name[32];
        size_t val_len = 0;
        cmap_value_types_t val_type;
        err = cmap_iter_next(conn, iter_h, key_name, &val_len, &val_type);
		if (err != CS_OK) {
			printf("Uh-oh: %s\n", get_error(err));
            break;
        }
		printf("This is a key_name: %s\n", key_name);
	}
	(void)cmap_iter_finalize (conn, iter_h);
	err = cmap_close(conn);
	return err;
}

int add_node(char *addr)
{
	int err;
	cmap_handle_t conn; //our connection
	
	err = cmap_init(conn);
	printf("%s added to cluster.\n", addr);
	err = cmap_close(conn);
	return err;
}

/* Using Brian's example code error printer here temporarily */
const char *get_error(int err) {
    switch(err) {
    case CS_OK: return "Success";
    case CS_ERR_LIBRARY: return "Library error";
    case CS_ERR_VERSION: return "Version error";
    case CS_ERR_INIT: return "Initialization error";
    case CS_ERR_TIMEOUT: return "Timeout";
    case CS_ERR_TRY_AGAIN: return "Resource temporarily unavailable";
    case CS_ERR_INVALID_PARAM: return "Invalid argument";
    case CS_ERR_NO_MEMORY: return "Not enough memory to completed the requested task";
    case CS_ERR_BAD_HANDLE: return "Bad handle";
    case CS_ERR_BUSY: return "Busy";
    case CS_ERR_ACCESS: return "Permission denied";
    case CS_ERR_NOT_EXIST: return "Not found";
    case CS_ERR_NAME_TOO_LONG: return "Name is too long";
    case CS_ERR_EXIST: return "Already exists";
    case CS_ERR_NO_SPACE: return "Insufficient memory";
    case CS_ERR_INTERRUPT: return "System call inturrupted by a signal";
    case CS_ERR_NOT_SUPPORTED: return "The requested protocol/functuality not supported";
    case CS_ERR_MESSAGE_ERROR: return "Incorrect auth message received";
    case CS_ERR_NAME_NOT_FOUND: return "Name not found";
    case CS_ERR_NO_RESOURCES: return "No resources";
    case CS_ERR_BAD_OPERATION: return "Bad operation";
    case CS_ERR_FAILED_OPERATION: return "Failed operation";
    case CS_ERR_QUEUE_FULL: return "Queue full";
    case CS_ERR_QUEUE_NOT_AVAILABLE: return "Queue not available";
    case CS_ERR_BAD_FLAGS: return "Bad flags";
    case CS_ERR_TOO_BIG: return "Too big";
    case CS_ERR_NO_SECTIONS: return "No sections";
    case CS_ERR_CONTEXT_NOT_FOUND: return "Context not found";
    case CS_ERR_TOO_MANY_GROUPS: return "Too many groups";
    case CS_ERR_SECURITY: return "Security error";
    default: return "Unknown error";     
    }
}