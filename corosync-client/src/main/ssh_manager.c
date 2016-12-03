#include "ssh_manager.h"

void free_channel(ssh_channel channel)
{
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);
}

void free_session(ssh_session session)
{
	ssh_disconnect(session);
	ssh_free(session);
}

int start_corosync(char *addr)
{
	ssh_session session;
    ssh_channel channel;
    int rc;
	int port;
	char buffer[1024];
    unsigned int nbytes;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		return CL_SSH_CONN_ERR;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		free_session(session);
		return CL_SSH_CONN_ERR;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		return CL_SSH_AUTH_ERR;;
	}
	//create command channel
    channel = ssh_channel_new(session);
    if(channel == NULL){
		free_session(session);
		return CL_SSH_CREATE_CHANNEL_ERR;
	}
	//open channel
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_OPEN_CHANNEL_ERR;
	}
	//execute command
    rc = ssh_channel_request_exec(channel, "corosync");
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_COMMAND_EXEC_FAIL;
	}
	//read in stuff (if anything)
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        //fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
	//success - free everything
    free_channel(channel);
    free_session(session);
    return CS_OK;
}

int stop_corosync(char *addr)
{
	ssh_session session;
    ssh_channel channel;
    int rc;
	int port;
	char buffer[1024];
    unsigned int nbytes;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		return CL_SSH_CONN_ERR;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		free_session(session);
		return CL_SSH_CONN_ERR;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		return CL_SSH_AUTH_ERR;;
	}
	//create command channel
    channel = ssh_channel_new(session);
    if(channel == NULL){
		free_session(session);
		return CL_SSH_CREATE_CHANNEL_ERR;
	}
	//open channel
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_OPEN_CHANNEL_ERR;
	}
	//execute command
    rc = ssh_channel_request_exec(channel, "corosync-cfgtool -H");
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_COMMAND_EXEC_FAIL;
	}
	//read in stuff (if anything)
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        //fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
	//success - free everything
    free_channel(channel);
    free_session(session);
    return CS_OK;
}

int start_corosync_timed(char *addr, long *cmd_start)
{
	ssh_session session;
    ssh_channel channel;
    int rc;
	int port;
	char buffer[1024];
    unsigned int nbytes;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		return CL_SSH_CONN_ERR;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		free_session(session);
		return CL_SSH_CONN_ERR;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		return CL_SSH_AUTH_ERR;;
	}
	//create command channel
    channel = ssh_channel_new(session);
    if(channel == NULL){
		free_session(session);
		return CL_SSH_CREATE_CHANNEL_ERR;
	}
	//open channel
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_OPEN_CHANNEL_ERR;
	}
	//execute command
	/////
	//get time right before sending request
	get_microtime(cmd_start);
	/////
    rc = ssh_channel_request_exec(channel, "corosync");
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_COMMAND_EXEC_FAIL;
	}
	//read in stuff (if anything)
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        //fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
	//success - free everything
    free_channel(channel);
    free_session(session);
    return CS_OK;
}

int stop_corosync_timed(char *addr, long *cmd_start)
{
	ssh_session session;
    ssh_channel channel;
    int rc;
	int port;
	char buffer[1024];
    unsigned int nbytes;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		return CL_SSH_CONN_ERR;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		free_session(session);
		return CL_SSH_CONN_ERR;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		return CL_SSH_AUTH_ERR;;
	}
	//create command channel
    channel = ssh_channel_new(session);
    if(channel == NULL){
		free_session(session);
		return CL_SSH_CREATE_CHANNEL_ERR;
	}
	//open channel
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_OPEN_CHANNEL_ERR;
	}
	//execute command
	/////get time right before sending request
	get_microtime(cmd_start);
	///////
    rc = ssh_channel_request_exec(channel, "corosync-cfgtool -H");
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		return CL_SSH_COMMAND_EXEC_FAIL;
	}
	//read in stuff (if anything)
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        //fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
	//success - free everything
    free_channel(channel);
    free_session(session);
    return CS_OK;
}

int restart_corosync(char *addr)
{
	int err;
	
	err = stop_corosync(addr);
	if(err != CS_OK){
		return err;
	}
	err = start_corosync(addr);
	if(err != CS_OK){
		return err;
	}
	return CS_OK;
}