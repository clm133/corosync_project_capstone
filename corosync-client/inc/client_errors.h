#ifndef CLIENT_ERRORS_H_DEFINED
#define CLIENT_ERRORS_H_DEFINED

#include <stdio.h>
#include <stdlib.h>

#include "cs_errors.h"

typedef enum {
	CL_FILE_NOT_FOUND = -1, 
	CL_WRONG_CMAP_TYPE = -2,
	CL_SSH_CONN_ERR = -3,
	CL_SSH_AUTH_ERR = -4,
	CL_SSH_CREATE_CHANNEL_ERR = -5,
	CL_SSH_OPEN_CHANNEL_ERR = -6,
	CL_SSH_COMMAND_EXEC_FAIL = -7,
	CL_NODEID_ERR = -8,
	CL_SFTP_ERR = -9,
} client_error_t;

const char *get_client_error(client_error_t err);
const char *get_error(int err);

#endif /* CLIENT_ERRORS_H */