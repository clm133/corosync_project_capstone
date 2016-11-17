#include "client_errors.h"

/*client Error Codes*/
const char *get_client_error(client_error_t err)
{
    switch(err){
    	case CL_FILE_NOT_FOUND: return "Unable to locate a file or directory";
		case CL_WRONG_CMAP_TYPE: return "Incorrect, unsupported, or unknown cmap_value_type_t";
		case CL_SSH_CONN_ERR: return "Error establishing ssh connection";
		case CL_SSH_AUTH_ERR: return "Error authenticating ssh conection";
		case CL_SSH_CREATE_CHANNEL_ERR: return "Error creating ssh channel";
		case CL_SSH_OPEN_CHANNEL_ERR: return "Error opening ssh channel";
		case CL_SSH_COMMAND_EXEC_FAIL: return "command executed via ssh failed";
		case CL_NODEID_ERR: return "Error obtaining/generating node id";
		case CL_SFTP_ERR: return "Error regarding sftp";
		default: return "Unknown error";
	}
}

const char *get_error(int err)
{
	if(err > 0){
		return get_cs_error(err);
	}
	else{
		return get_client_error(err);
	}
}