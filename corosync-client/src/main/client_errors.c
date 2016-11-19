#include "client_errors.h"

/*Corosync Error Codes*/
const char * get_cs_error(cs_error_t err)
{
    switch(err){
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
		case CL_SFTP_ERR: return "Unknown error regarding sftp";
		case CL_SFTP_AUTH: return "sftp authentication error";
		case CL_SFTP_CONN: return "sftp connection error";
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